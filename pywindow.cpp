#define NPY_NO_DEPRECATED_API 7

#include <Python.h>
#include <structmember.h>
#include <numpy/arrayobject.h>

#include <vector>

#include <cstring>
#include <stdlib.h>     /* rand */

#include <stdio.h>
#include <math.h>
#include <string.h> 
#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>


const char *Vertex_Shader_Source = 
  "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;"
  "layout (location = 1) in vec2 aTexCoord;"
  "out vec2 UV;"
  "void main() {"
  "  gl_Position = vec4( aPos, 1 );"
	"  UV = aTexCoord;"
  "}" ;

const char *Fragment_Shader_Source = 
  "#version 330 core\n"
  "out vec4 FragColor;"
  "in vec2 UV;"
  "uniform sampler2D tex1;"
  "void main(){"
  "    FragColor = texture( tex1, UV ) ; "
  "}" ;

/********************************************************************
 * 
 * C defintion for Python class
 * 
********************************************************************/

typedef struct {
  PyObject_HEAD // no semicolon
  int width ;
  int height ;
  char window_title[128] ;
} PyWindow ;

GLFWwindow* window ;
GLuint shaderProgram ;
GLuint texture ;
GLuint VBO, VAO, EBO;

void createVertexData( PyWindow *self ) {
    float vertices[] = {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
    } ;
 
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    } ;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}



GLuint createTexture( PyWindow *self ) {
  GLuint textureID;
  glGenTextures(1, &textureID);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

  return textureID ;
}


GLuint createProgram( PyWindow *self ) {
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &Vertex_Shader_Source, NULL);
  glCompileShader(vertexShader);
    // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success){
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf( "ERROR::SHADER::VERTEX::COMPILATION_FAILED, %s\n", infoLog ) ;
  }

  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &Fragment_Shader_Source, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      printf( "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED, %s\n", infoLog ) ;
  }
  // link shaders
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      printf( "ERROR::SHADER::PROGRAM::LINKING_FAILED, %s\n", infoLog ) ;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  return shaderProgram ;
}


void renderScene( PyWindow *self ) {
  glfwMakeContextCurrent(window) ;

  glUseProgram( shaderProgram ) ;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

  glActiveTexture( GL_TEXTURE0 ) ;
  glBindTexture( GL_TEXTURE_2D, texture ) ;

  glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glUseProgram( 0 ) ;

  glFlush();

  glfwSwapBuffers( window );
}

static void resize( GLFWwindow *window, int width, int height ) {
  glfwMakeContextCurrent( window ) ;
  PyWindow *self = reinterpret_cast<PyWindow*>( glfwGetWindowUserPointer( window ) ) ;
  self->height = height ;
  self->width = width ;
  glViewport(0, 0, width, height) ;
}

static void openwin( PyWindow *self ) {
    glfwInit();

    // Create window
    window = glfwCreateWindow( self->width, self->height, self->window_title, NULL, NULL);
    
    glfwMakeContextCurrent( window ) ;
    glfwSetWindowUserPointer( window, self ) ;

    glfwSetWindowSizeCallback( window, resize ) ;
    // Enable Z-buffer depth test & textures
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D) ;

    shaderProgram = createProgram( self ) ;
    createVertexData( self ) ;
    texture = createTexture( self ) ;
    GLint loc = glGetUniformLocation( shaderProgram, "tex1") ;
    glUniform1i( loc, 0 );    
}


/********************************************************************
*
* Allocation/deallocation & __init__ definitions 
*
********************************************************************/

static void PyWindow_dealloc(PyWindow *self) {
  Py_TYPE(self)->tp_free((PyObject *)self);
  glfwDestroyWindow( window ) ;
}

static PyObject *PyWindow_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  PyWindow *self = (PyWindow *)type->tp_alloc(type, 0);
  return (PyObject *)self;
}

static int PyWindow_init(PyWindow *self, PyObject *args, PyObject *kwargs ) {
// What am I looking for?
  static char *kwlist[] = {
        "width",
        "height",
        "title",
        NULL
  } ;
// Defaults
  self->width = 300 ;
  self->height = 300 ;
  char *tmp_title = "Py Window" ;
// parse keywords
  if( PyArg_ParseTupleAndKeywords(args, kwargs, "|iis", kwlist, 
            &self->width, &self->height, &tmp_title ) ) {  
    strncpy( self->window_title, tmp_title, sizeof( self->window_title) ) ;
  } else {
// in case of error
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_Exception, "Constructor has invalid arguments" ) ;
    PyGILState_Release(gstate);
    return 1 ;
  }

  openwin( self ) ; 

// Success return
  return 0;
}


/********************************************************************
*
* Member attributes exposed from C definition
*
********************************************************************/

static PyMemberDef PyWindow_members[] = {
    { "width", T_UINT, offsetof(PyWindow, width) , READONLY, "The window X size" },
    { "height", T_UINT, offsetof(PyWindow, height) , READONLY, "The window Y size" },
    {NULL, 0, 0, 0, NULL}
};


/********************************************************************
*
* Printing type functions
*
********************************************************************/

static PyObject *PyWindow_repr( PyWindow *self ) {
  char rc[10] ;
  strncpy( rc, self->window_title, sizeof(self->window_title) ) ;
  return Py_BuildValue( "s", rc ) ;
};


/********************************************************************
 *
 * Regular Methods
 * 
 ********************************************************************/

static PyObject *set_image( PyWindow *self, PyObject *args ) {

  PyObject *arg = PyTuple_GET_ITEM( args, 0 ) ; 

  if( !PyArray_Check( arg ) ) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_Exception, "set_image requires a numpy array" ) ;
    PyGILState_Release(gstate);
  } else {
    PyArrayObject *arr = reinterpret_cast<PyArrayObject*> ( arg );
    int ndims = PyArray_NDIM( arr ) ;
    npy_intp *shape = PyArray_DIMS( arr ) ;
    if( ndims != 3 || shape[2] != 4 ) {
      PyGILState_STATE gstate = PyGILState_Ensure();
      PyErr_SetString(PyExc_Exception, "set_image requires a numpy array of shape [width,height,4] to hold RGBA bytes" ) ;
      PyGILState_Release(gstate);
    } else {
      glfwMakeContextCurrent(window) ;

      // "Bind" the newly created texture : all future texture functions will modify this texture
      glBindTexture(GL_TEXTURE_2D, texture ) ;

      void *data = PyArray_DATA( arr ) ;
      // Give the image to OpenGL
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shape[0], shape[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, data ) ;
    }
  }
  Py_RETURN_NONE ;
}


static PyObject *show( PyWindow *self ) {
    int closing = glfwWindowShouldClose(window) ;
    if( !closing ) {
        renderScene( self ) ;
        /* Poll for and process events */
        glfwPollEvents();
    }
    return Py_BuildValue( "b", !closing ) ;
}


static PyMethodDef PyWindow_methods[] = {
  {"set_image", (PyCFunction)set_image, METH_VARARGS, "Sets data for the screen image, should be RGBA format, and sizeof screen"},
  {"show", (PyCFunction)show, METH_NOARGS, "Process the window events"},
  {NULL, NULL, 0, NULL}
};

/********************************************************************
 * 
 * Main Type Definition
 * 
 ********************************************************************/
static PyTypeObject PyWindowType = {
    PyVarObject_HEAD_INIT(NULL, 0) 
    "pywindow.PyWindow",                    /* tp_name */
    sizeof(PyWindow),                       /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)PyWindow_dealloc,           /* tp_dealloc */
    0,                                        /* tp_print */
    0,                                        /* tp_getattr */
    0,                                        /* tp_setattr */
    0,                                        /* tp_reserved */
    (reprfunc)PyWindow_repr,                /* tp_repr */
    0,                                        /* tp_as_number */
    0,                                        /* tp_as_sequence */
    0,                                        /* tp_as_mapping */
    0,                                        /* tp_hash  */
    0,                                        /* tp_call */
    0,                                        /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    "PyWindow objects",                     /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    // (getiterfunc)PyWindow_getiter,          /* tp_iter */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */
    PyWindow_methods,                       /* tp_methods */
    PyWindow_members,                       /* tp_members */
    0,                                        /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)PyWindow_init,                /* tp_init */
    0,                                        /* tp_alloc */
    PyWindow_new                            /* tp_new */
};


/*****************************************************************
 * 
 * Main Module Definition / Initialization
 * 
 *****************************************************************/
static struct PyModuleDef pywindow_definition = {
    PyModuleDef_HEAD_INIT,
    "pywindow",
    "example module containing PyWindow class",
    -1,
    NULL,
};

PyMODINIT_FUNC PyInit_pywindow(void) {
  import_array() ;  // Using numpy funcs? MUST call this first 

  srand48( getpid() ) ;
  srand( getpid() ) ;
  Py_Initialize();
  PyObject *m = PyModule_Create(&pywindow_definition);

  if (PyType_Ready(&PyWindowType) < 0)
    return NULL;

  Py_INCREF(&PyWindowType);   // class is in use

  PyModule_AddObject(m, "PyWindow", (PyObject *)&PyWindowType);

  return m;
}

