#ifndef BOARDGRAPHICSNAME_H
#define BOARDGRAPHICSNAME_H

class BoardGraphicsView : public QGraphicsView
{
public:
    BoardGraphicsView(QWidget* mainWin) : QGraphicsView(mainWin) {}

    void mousePressEvent (QMouseEvent*);

private:

    //mouseReleaseEvent
};


#endif // BOARDGRAPHICSNAME_H
