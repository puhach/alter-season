#include "conversionselectordlg.h"

#include <QVBoxLayout>
//#include <QPushButton>
#include <QCommandLinkButton>

ConversionSelectorDlg::ConversionSelectorDlg(QWidget* parent)
	: QDialog(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint)
{
	QCommandLinkButton* btnSummer2Winter = new QCommandLinkButton(tr("Summer to Winter"),
		tr("Choose this option to convert your summer image to the winter style."));

	QCommandLinkButton* btnWinter2Summer = new QCommandLinkButton(tr("Winter to Summer"),
		tr("Choose this option to convert your winter image to the summer style."));

	QVBoxLayout* vlayout = new QVBoxLayout;
	vlayout->addWidget(btnSummer2Winter);
	vlayout->addWidget(btnWinter2Summer);
	setLayout(vlayout);
}
