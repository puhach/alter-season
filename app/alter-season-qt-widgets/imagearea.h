#ifndef IMAGEAREA_H
#define IMAGEAREA_H

#include <QLabel>

class ImageArea : public QLabel
{
public:
	ImageArea(const QString &text, QWidget* parent = nullptr);

	//void showMessage(const QString& message, int duration);

protected:
	//virtual void dragEnterEvent(QDragEnterEvent* event) override;
	//virtual void dropEvent(QDropEvent* event) override;

private:
	QString text;
};	// ImageArea

#endif  // IMAGEAREA_H

