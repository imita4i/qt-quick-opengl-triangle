#ifndef GLESFIGURE_H
#define GLESFIGURE_H

#include <QOpenGLShader>
#include <QOpenGLFunctions>

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>

class GLESFigure : public QQuickItem, protected QOpenGLFunctions {
    Q_OBJECT

    public:
        GLESFigure();
        ~GLESFigure();

    public slots:
        void render();
        void synchronization();

    private slots:
        void onWindowChanged(QQuickWindow *window);
        // needed for sinc Open Gl and qml

    protected:
        virtual void mouseReleaseEvent(QMouseEvent *event);
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);
        // handle mouse event for control rotation

    private:
        float rotateAngle = 0;
        bool mousePressed = false;
        QSize viewportSize = QSize(0, 0);
        QPoint previousPosition;
        QOpenGLShaderProgram *program = NULL;

        GLfloat *verticeColor;
        GLfloat *vertices;
        GLfloat *colors;

        void initialization();
        void updateColor();
        void updateView();
        void updateRotateAngle(float diff);
};

#endif // GLESFIGURE_
