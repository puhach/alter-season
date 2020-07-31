#include "conversionselectordlg.h"

#include <QVBoxLayout>
//#include <QPushButton>
#include <QCommandLinkButton>

ConversionSelectorDlg::ConversionSelectorDlg(QWidget* parent)
	: QDialog(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint)
{
	QCommandLinkButton* btnSummer2Winter = new QCommandLinkButton(tr("Summer to Winter"),
		tr("Choose this option to convert your summer image to the winter style."));
	connect(btnSummer2Winter, &QCommandLinkButton::clicked, this, [this] { done(Summer2Winter);  });

	QCommandLinkButton* btnWinter2Summer = new QCommandLinkButton(tr("Winter to Summer"),
		tr("Choose this option to convert your winter image to the summer style."));
	connect(btnWinter2Summer, &QCommandLinkButton::clicked, this, [this] { done(Winter2Summer); });

	QVBoxLayout* vlayout = new QVBoxLayout;
	vlayout->addWidget(btnSummer2Winter);
	vlayout->addWidget(btnWinter2Summer);
	setLayout(vlayout);
}

//void ConversionSelectorDlg::reject()
//{
//
//}