#include "DefaultNodeEditor.h"
#include "Node.h"
#include "Logger.h"

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

DefaultNodeEditor::DefaultNodeEditor(Node *_node, QWidget *parent)
	: QWidget(parent)
	, m_node(_node)
{
	m_layout = new QGridLayout();
	for (int i = 0; i < node()->parmCount(); ++i)
	{
		addParmInput(i);
	}
	setLayout(m_layout);

	connect(node(), &Node::parmChanged, this, &DefaultNodeEditor::updateParm);
}

void DefaultNodeEditor::updateParm(int parm)
{
	if (parm < 0 || parm >= node()->parmCount())
	{
		ERR_LOG << "Invalid parm index: #" << parm;
		return;
	}

	// Add missing parameter inputs
	while (parm >= m_inputs.size())
	{
		int n = static_cast<int>(m_inputs.size());
		addParmInput(n);
	}

	// TODO: Remove extra parameter inputs

	QWidget *input = m_inputs[parm];
	switch (node()->parmType(parm))
	{
	case StringType:
	{
		QLineEdit *w = static_cast<QLineEdit*>(input);
		w->setText(node()->parmRawValue(parm).toString());
		break;
	}
	case IntType:
	{
		QSpinBox *w = static_cast<QSpinBox*>(input);
		w->setValue(node()->parmRawValue(parm).toInt());
		break;
	}
	case EnumType:
	{
		QComboBox *w = static_cast<QComboBox*>(input);
		w->setCurrentIndex(node()->parmRawValue(parm).toInt());
		break;
	}
	default:
	{
		QLineEdit *w = static_cast<QLineEdit*>(input);
		w->setText(node()->parmRawValue(parm).toString());
		break;
	}
	}
}

void DefaultNodeEditor::addParmInput(int parm)
{
	QLabel *label = new QLabel(node()->parmName(parm));
	m_layout->addWidget(label, parm, 0);

	QWidget *input;
	switch (node()->parmType(parm))
	{
	case StringType:
	{
		QLineEdit *w = new QLineEdit();
		w->setText(node()->parmRawValue(parm).toString());
		connect(w, &QLineEdit::textEdited, [=](const QString &text) { node()->setParm(parm, text); });
		input = w;
		break;
	}
	case IntType:
	{
		QSpinBox *w = new QSpinBox();
		w->setMaximum(99999999);
		w->setValue(node()->parmRawValue(parm).toInt());
		connect(w, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) { node()->setParm(parm, value); });
		input = w;
		break;
	}
	case EnumType:
	{
		QComboBox *w = new QComboBox();
		for (int j = 0; j < node()->parmMenuCount(parm); ++j)
		{
			w->addItem(node()->parmMenuLabel(parm, j));
		}
		w->setCurrentIndex(node()->parmRawValue(parm).toInt());
		connect(w, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) { node()->setParm(parm, index); });
		input = w;
		break;
	}
	default:
	{
		QLineEdit *w = new QLineEdit();
		w->setText(node()->parmRawValue(parm).toString());
		connect(w, &QLineEdit::textEdited, [=](const QString &text) { node()->setParm(parm, text); });
		input = w;
		break;
	}
	}
	m_layout->addWidget(input, parm, 1);
	m_inputs.push_back(input);
}
