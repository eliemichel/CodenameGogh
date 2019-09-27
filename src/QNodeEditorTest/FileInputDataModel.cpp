#include "FileInputDataModel.h"

#include <QtCore/QJsonValue>
#include <QtGui/QIntValidator>

#include "VideoStreamData.h"

FileInputDataModel::
FileInputDataModel()
  : _lineEdit(new QLineEdit())
{
  _lineEdit->setValidator(new QIntValidator());

  _lineEdit->setMaximumSize(_lineEdit->sizeHint());

  connect(_lineEdit, &QLineEdit::textChanged,
          this, &FileInputDataModel::onTextEdited);

  _lineEdit->setText("0");
}


QJsonObject
FileInputDataModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();

  modelJson["file"] = _lineEdit->text();

  return modelJson;
}


void
FileInputDataModel::
restore(QJsonObject const &p)
{
  QJsonValue v = p["file"];

  if (!v.isUndefined())
  {
    _lineEdit->setText(v.toString());
  }
}


unsigned int
FileInputDataModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 0;
      break;

    case PortType::Out:
      result = static_cast<unsigned int>(_streams.size());

    default:
      break;
  }

  return result;
}


void
FileInputDataModel::
onTextEdited(QString const &string)
{
  Q_UNUSED(string);

  bool ok = false;

  int number = _lineEdit->text().toInt(&ok);
  if (number < 0) number = 0;
  if (number > 5) number = 5;

  if (ok)
  {
	  setStreamCount(number);
  }
  else
  {
    Q_EMIT dataInvalidated(0);
  }
}

NodeDataType
FileInputDataModel::
dataType(PortType, PortIndex) const
{
  return VideoStreamData().type();
}

std::shared_ptr<NodeData>
FileInputDataModel::
outData(PortIndex port)
{
	return _streams[port];
}

void
FileInputDataModel::
setStreamCount(int number)
{
  _streams.clear();
  for (int i = 0 ; i < number ; ++i) {
    _streams.push_back(std::make_shared<VideoStreamData>(0, i));
  }
  Q_EMIT portCountChanged();
  for (int i = 0 ; i < number ; ++i) {
    Q_EMIT dataUpdated(i);
  }
}
