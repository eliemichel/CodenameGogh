#ifndef H_NODE
#define H_NODE

#include "SlotIndex.h"
#include "Parameter.h"

#include <QObject>
#include <QString>
#include <QVariant>
#include <QModelIndex>

#include <set>
#include <vector>
#include <string>
#include <utils/stringlist.h>
#include <utils/strutils.h>


class EnvModel;
class NodeGraphModel;
class QWidget;

/* Type defined to transmit a pair (input_file, stream_number)
	 which characterizes a stream.
	*/
typedef std::pair<std::string, int> filestream;

enum StreamType
{
	VideoStream,
	AudioStream,
	SubtitleStream,
	DataStream,
};

/**
 * This structure is transmitted among the graph nodes while building the
 * render command. Feel free to add any field required to properly build
 * the command.
 */
struct RenderCommand {
	// == Input ==
	// Variables used for input treatment

	// OutputNode's smart renaming : keys (like : "codec", "scale") associated to current nodes values (like : "h264", "1920x1080")
	std::map<std::string, std::string> env;

	// Current filestream, mainly used for MixNode:
	filestream fs;

	// Current settings, mainly used for MixNode:
	stringlist cs;

	//Input sources
	stringlist sources;

	//Input filestreams
	std::vector<filestream> inputs;

	// StreamType by filestream
	std::map<filestream, StreamType> streams;

	// ------------
	// == Output ==
	// Variables used for output command building

	// Filestream by output streams
	std::map<int, filestream> outputs;

	//Output streams names
	std::map<int, std::string> names;

	// Settings by output
	std::map<int, stringlist> settings;

 	// Final command to send to ffmpeg, must be filled only int the OuputNode
 	stringlist cmd;

	// ------------
	// == Getters ==
	int source_id (filestream filestream){
		for (int i = 0; i < sources.size(); i++)
		{
			if(filestream.first == sources[i])
			{
				return i;
			}
		}
		return 0;
	}

	// ------------
	// == Other ==
	// error message that may be filled when returning false in buildRenderCommand
	std::string err;
};

class Node : public QObject
{
	Q_OBJECT
public:
	class SlotEvent
	{
		friend class Node;

	public:
		/// index in inputSlots() or outputSlots() of the slot that has been connected
		int slotIndex() const { return m_slotIndex; };
		/// true if the connected slot is an input slot, false if it is an output
		bool isInputSlot() const { return m_isInputSlot; };

	private:
		SlotEvent(int slotIndex, bool isInputSlot)
			: m_slotIndex(slotIndex), m_isInputSlot(isInputSlot)
		{}

	private:
		int m_slotIndex;
		bool m_isInputSlot;
	};

public:
	// TODO: make these private and add accessors
	int type;
	float x, y;
	std::string name;
	std::vector<SlotIndex> inputLinks;
	std::vector<std::set<SlotIndex>> outputLinks;

public:
	explicit Node(QObject *parent = nullptr);

	/**
	 * Create an editor widget to operate on this node's parms.
	 * This is the only method that handle graphical objects.
	 */
	virtual QWidget *createEditor(QWidget *parent = nullptr);

	void setEnvModel(EnvModel *envModel) { m_envModel = envModel; }
	void setGraphModel(NodeGraphModel *model) { m_graphModel = model; }

	int nodeIndex() const { return m_nodeIndex; }
	void setNodeIndex(int index) { m_nodeIndex = index; }

public:
	// parm model
	virtual int parmCount() const { return 0; }
	virtual QString parmName(int parm) const { return QString(); }
	virtual ParmType parmType(int parm) const { return NoneType; }
	virtual QVariant parmRawValue(int parm) const { return QVariant(); }

	// menu items are used for parm of type EnumType
	virtual int parmMenuCount(int parm) const { return 0; }
	virtual QString parmMenuLabel(int parm, int menu) const { return QString(); }
	virtual QVariant parmMenuValue(int parm, int menu) const { return menu; }

	/// When overriding, think about firing parmChanged(int parm) events when parameter change is accepted
	virtual bool setParm(int parm, QVariant value) { return false; }

	QString parmEvalAsString(int parm) const;
	int parmEvalAsInt(int parm) const;
	bool parmEvalAsBool(int parm) const;

	// slot structure read
	int inputSlotCount() const { return static_cast<int>(inputLinks.size()); }
	int outputSlotCount() const { return static_cast<int>(outputLinks.size()); }

signals:
	void parmChanged(int parm);


protected:
	// slot structure write
	void newInputSlot();
	void newOutputSlot();

	void removeOutputSlots();

	Connection inputConnection(int inputSlotIndex);
	std::set<Connection> outputConnection(int outputSlotIndex);

	EnvModel * envModel() const { return m_envModel; }
	NodeGraphModel *graphModel() const { return m_graphModel; }

public:

	/**
	 * Function that contains the logic of the node. This must be reimplemented
	 * in each node and is called when building the render command.
	 */
	virtual bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const { return true; }

	/**
	 * Convenience function calling buildRenderCommand() on the source output slot
	 * connected to the input at index inputIndex.
	 */
	bool parentBuildRenderCommand(int inputIndex, RenderCommand & cmd) const;

	virtual void read(QDataStream & stream);
	virtual void write(QDataStream & stream) const;

	/**
	 * This is used in graphics view to initiate slotConnectEvent() propagation.
	 * This should ultimately be modified so do not use this method anywhere else
	 * Reserved to NodeGraphModel
	 */
	void fireSlotConnectEvent(int slotIndex, bool isInput);
	void fireSlotDisconnectEvent(int slotIndex, bool isInput);

protected:
	virtual void slotConnectEvent(SlotEvent *event) {}
	virtual void slotDisconnectEvent(SlotEvent *event) {}

private:
	EnvModel *m_envModel;
	NodeGraphModel *m_graphModel;
	int m_nodeIndex;
};

#endif // H_NODE
