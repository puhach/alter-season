#ifndef CONVERSIONSELECTORDLG_H
#define CONVERSIONSELECTORDLG_H

#include <QDialog>

class QCommandLinkButton;

class ConversionSelectorDlg : public QDialog
{
public:

	enum
	{
		Summer2Winter = 100,
		Winter2Summer = 200
	};

	ConversionSelectorDlg(QWidget* parent = nullptr);

public slots:
	//virtual void reject();

private:

};

#endif // CONVERSIONSELECTORDLG_H