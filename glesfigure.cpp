#include "glesfigure.h"

GLESFigure::GLESFigure() {
    verticeColor = new GLfloat[3];
    verticeColor[0] = 1.0f; // R
    verticeColor[1] = 1.0f; // G
    verticeColor[2] = 1.0f; // B

    vertices = new GLfloat[6]; // two coordinate to vertical
    vertices[0] =  0.0f; vertices[1] =  1.0f;
    vertices[2] = -1.0f; vertices[3] = -1.0f;
    vertices[4] =  1.0f; vertices[5] = -1.0f;

    colors = new GLfloat[9]; // color for all vertical
    colors[0] = 0.0f; colors[1] = 0.0f; colors[2] = 0.0f;
    colors[3] = 0.0f; colors[4] = 0.0f; colors[5] = 0.0f;
    colors[6] = 0.0f; colors[7] = 0.0f; colors[8] = 0.0f;

    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

    connect(this, SIGNAL(windowChanged(QQuickWindow*)),
            this, SLOT(onWindowChanged(QQuickWindow*)));
}

GLESFigure::~GLESFigure() {
    delete program;
}

//! color change depending on the coefficient from 0 to 120 according to RGB
void GLESFigure::updateColor() {
    if (rotateAngle < 120) verticeColor[0] = rotateAngle / 120.0f;
    else if (rotateAngle < 240) {
        verticeColor[1] = (rotateAngle - 120.0f) / 120.0f;
    }
    else verticeColor[2] = (rotateAngle - 240.0f) / 120.0f;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            colors[i * 3 + j] = verticeColor[j];
        }
    }
}

void GLESFigure::initialization() {
    initializeOpenGLFunctions();
    program = new QOpenGLShaderProgram();

    program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                     "attribute highp vec4 verts;\n"
                                     "attribute lowp vec4 colors;\n"
                                     "varying lowp vec4 color;\n"
                                     "uniform highp mat4 matrix;\n"
                                     "void main() {\n"
                                     "   color = colors;\n"
                                     "   gl_Position = matrix * verts;\n"
                                     "}\n");
    program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                     "varying lowp vec4 color;\n"
                                     "void main() {\n"
                                     "   gl_FragColor = color;\n"
                                     "}\n");
    program->link();
}

void GLESFigure::render() {
    if (!program) initialization(); // first render

    if (program->bind()) {
        int vertsLocation = program->attributeLocation("verts");
        int colorsLocation = program->attributeLocation("colors");
        int matrixUniformLocation = program->uniformLocation("matrix");
        program->enableAttributeArray(vertsLocation);
        program->enableAttributeArray(colorsLocation);
        program->setAttributeArray(vertsLocation, GL_FLOAT, vertices, 2);
        program->setAttributeArray(colorsLocation, GL_FLOAT, colors, 3);
        glViewport(0, 0, viewportSize.width(), viewportSize.height());
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        updateColor();

        QMatrix4x4 matrix;
        matrix.perspective(45.0f, 1.0f, 1.0f, 0.0f);
        matrix.translate(0.0f, 0.0f, -3.0f);
        matrix.rotate(rotateAngle, 0.0f, 1.0f, 0.0f);
        program->setUniformValue(matrixUniformLocation, matrix);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        program->disableAttributeArray(vertsLocation);
        program->disableAttributeArray(colorsLocation);
        program->release();
    }
    else qDebug() << "program Open Gl can't bind";

}

void GLESFigure::onWindowChanged(QQuickWindow *window) {
    if (window) {
        connect(window, SIGNAL(beforeSynchronizing()),
                this, SLOT(synchronization()), Qt::DirectConnection);
        window->setClearBeforeRendering(false);
    }
}

void GLESFigure::synchronization() {
    connect(window(), SIGNAL(beforeRendering()),
            this, SLOT(render()), Qt::DirectConnection);
    viewportSize = window()->size() * window()->devicePixelRatio();
}

void GLESFigure::mousePressEvent(QMouseEvent *event) {
    QQuickItem::mousePressEvent(event);
    if (event->buttons() == Qt::LeftButton) {
        previousPosition = event->pos();
        mousePressed = true;
        event->accept();
    }
}

void GLESFigure::mouseReleaseEvent(QMouseEvent *event) {
    QQuickItem::mouseReleaseEvent(event);
    if (event->buttons() == Qt::LeftButton) {
        mousePressed = false;
        event->accept();
    }
}

void GLESFigure::mouseMoveEvent(QMouseEvent *event) {
    QQuickItem::mouseMoveEvent(event);
    if (mousePressed) {
        QPoint currentPosition = event->pos();
        int current_x = currentPosition.x();
        int previous_x = previousPosition.x();
        updateRotateAngle(current_x - previous_x);
        updateView();
        previousPosition = currentPosition;
    }
}

void GLESFigure::updateView() {
    render();
    if (window()) window()->update();
}

//! Preventing overflow and negative values
void GLESFigure::updateRotateAngle(float diff) {
    rotateAngle += diff;
    if (rotateAngle >= 360) rotateAngle -= 360;
    else if (rotateAngle < 0) rotateAngle = 360 - rotateAngle;
}
