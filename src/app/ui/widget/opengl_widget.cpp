#include "opengl_widget.h"

#include <iostream>
#include <QMouseEvent>
#include "../../network/network_out_message.h"

opengl_widget::opengl_widget(QWidget* parent) :
    QOpenGLWidget(parent),
    m_shader(),
    m_texture()
{
    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
}

void opengl_widget::initializeGL()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_texture.create();
    m_texture.bind();
}

void opengl_widget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void opengl_widget::mouseMoveEvent(QMouseEvent* event)
{
    network_out_mousepos_update(event->x(), event->y(), this->width(), this->height()).send();
}

void opengl_widget::keyPressEvent(QKeyEvent* event)
{
    network_out_keyupdate(true, event->nativeVirtualKey()).send();
}

void opengl_widget::keyReleaseEvent(QKeyEvent *event)
{
    network_out_keyupdate(false, event->nativeVirtualKey()).send();
}

void opengl_widget::mousePressEvent(QMouseEvent* event)
{
    network_out_mousebtn_update(true, event->button()).send();
}

void opengl_widget::mouseReleaseEvent(QMouseEvent* event)
{
    network_out_mousebtn_update(false, event->button()).send();
}

void opengl_widget::wheelEvent(QWheelEvent* event)
{
    network_out_wheel_rotation(event->delta()).send();
}

void opengl_widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    m_texture.bind();

    glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex2f(-1, -1);
        glTexCoord2f(1, 1);
        glVertex2f(1, -1);
        glTexCoord2f(1, 0);
        glVertex2f(1, 1);
        glTexCoord2f(0, 0);
        glVertex2f(-1, 1);
    glEnd();
}

void opengl_widget::update_texture(frame* f)
{
    m_texture.bind();
    m_texture.update_data(f->data());
    delete f;

    emit update();
}
