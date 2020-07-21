#ifndef IMAGEAREA_H
#define IMAGEAREA_H

#include <QLabel>

class ImageArea : public QLabel
{
public:
	// TODO: add a title
	ImageArea(QWidget* parent = nullptr);

protected:
	//virtual void dragEnterEvent(QDragEnterEvent* event) override;
	//virtual void dropEvent(QDropEvent* event) override;
};	// ImageArea

#endif  // IMAGEAREA_H

