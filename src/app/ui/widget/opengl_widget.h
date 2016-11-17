#pragma once

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "../../gfx/frame.h"
#include "../../gfx/gl_texture.h"
#include <queue>

class opengl_widget : public QOpenGLWidget
{
    Q_OBJECT
public:
    opengl_widget(QWidget* parent = nullptr);

public slots:
    void paintGL() override;

    void update_texture(frame* f);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    gl_texture m_texture;
    QOpenGLShaderProgram m_shader;
    QOpenGLBuffer m_vertex_buffer;
    QOpenGLVertexArrayObject m_vao;
};
