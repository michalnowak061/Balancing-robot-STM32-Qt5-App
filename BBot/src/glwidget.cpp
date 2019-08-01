#include "glwidget.h"

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_xRot = 0;
    m_yRot = 0;
    m_zRot = 0;
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GLWidget::~GLWidget()
{
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

QSize GLWidget::minimumSizeHint() const
{
    return QSize(500, 500);
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

QSize GLWidget::sizeHint() const
{
    return QSize(500, 500);
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void qNormalizeAngle(double &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GLWidget::setXRotation(double angle)
{
    //qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        update();
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GLWidget::setYRotation(double angle)
{
    //qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        update();
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GLWidget::setZRotation(double angle)
{
    //qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        update();
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GLCreateCube(float x, float y, float z) {

    glPushMatrix();
    glScalef(x,y,z);

    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(  0.5, -0.5, -0.5);
    glVertex3f(  0.5, -0.5, -0.5);
    glVertex3f( -0.5,  0.5, -0.5);
    glVertex3f( -0.5, -0.5, -0.5);
    glEnd();

    // White side
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(  0.5, -0.5,  0.5);
    glVertex3f(  0.5,  0.5,  0.5);
    glVertex3f( -0.5,  0.5,  0.5);
    glVertex3f( -0.5, -0.5,  0.5);
    glEnd();

    // Purple side
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(  0.5, -0.5, -0.5);
    glVertex3f(  0.5,  0.5, -0.5);
    glVertex3f(  0.5,  0.5,  0.5);
    glVertex3f(  0.5, -0.5,  0.5);
    glEnd();

    // Green side
    glBegin(GL_POLYGON);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f( -0.5, -0.5,  0.5);
    glVertex3f( -0.5,  0.5,  0.5);
    glVertex3f( -0.5,  0.5, -0.5);
    glVertex3f( -0.5, -0.5, -0.5);
    glEnd();

    // Blue side
    glBegin(GL_POLYGON);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(  0.5,  0.5,  0.5);
    glVertex3f(  0.5,  0.5, -0.5);
    glVertex3f( -0.5,  0.5, -0.5);
    glVertex3f( -0.5,  0.5,  0.5);
    glEnd();

    // Red side
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(  0.5, -0.5, -0.5);
    glVertex3f(  0.5, -0.5,  0.5);
    glVertex3f( -0.5, -0.5,  0.5);
    glVertex3f( -0.5, -0.5, -0.5);
    glEnd();
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GLWidget::initializeGL() {

    // Enable background color
    glClearColor(static_cast<GLclampf>(0.2),
                 static_cast<GLclampf>(0.2),
                 static_cast<GLclampf>(0.2),
                 1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GLWidget::paintGL() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(2,0,5, 0,0,0, 0,1,0);

    glRotated(m_xRot,  1,0,0);
    glRotated(m_yRot,  0,1,0);
    glRotated(m_zRot,  0,0,1);

    GLCreateCube(2.5, 3.0, 0.75);
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GLWidget::resizeGL(int w, int h) {

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);

    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;
    // Reset projection
    glLoadIdentity();
    // Set perspective projection
    gluPerspective(fov, aspect, zNear, zFar);
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
