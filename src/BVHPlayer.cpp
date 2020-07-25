
#ifdef  WIN32
	#include <windows.h>
#endif

#include <GL/glut.h>
#include <stdio.h>
#include "BVH.h"

static float   camera_yaw = 0.0f;      
static float   camera_pitch = -20.0f;  
static float   camera_distance = 5.0f; 

static int     drag_mouse_r = 0; 
static int     drag_mouse_l = 0; 
static int     drag_mouse_m = 0; 
static int     last_mouse_x, last_mouse_y;

static int     win_width, win_height;



bool   on_animation = true;

float  animation_time = 0.0f;

int    frame_no = 0;

BVH *   bvh = NULL;



void  drawMessage( int line_no, const char * message )
{
	int   i;
	if ( message == NULL )
		return;

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0.0, win_width, win_height, 0.0 );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );

	glColor3f( 1.0, 0.0, 0.0 );
	glRasterPos2i( 8, 24 + 18 * line_no );
	for ( i=0; message[i]!='\0'; i++ )
		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, message[i] );

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}


void  display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef( 0.0, 0.0, - camera_distance );
	glRotatef( - camera_pitch, 1.0, 0.0, 0.0 );
	glRotatef( - camera_yaw, 0.0, 1.0, 0.0 );
	glTranslatef( 0.0, -0.5, 0.0 );

	float  light0_position[] = { 10.0, 10.0, 10.0, 1.0 };
	glLightfv( GL_LIGHT0, GL_POSITION, light0_position );

	float  size = 1.5f;
	int  num_x = 10, num_z = 10;
	double  ox, oz;
	glBegin( GL_QUADS );
		glNormal3d( 0.0, 1.0, 0.0 );
		ox = -(num_x * size) / 2;
		for ( int x=0; x<num_x; x++, ox+=size )
		{
			oz = -(num_z * size) / 2;
			for ( int z=0; z<num_z; z++, oz+=size )
			{
				if ( ((x + z) % 2) == 0 )
					glColor3f( 1.0, 1.0, 1.0 );
				else
					glColor3f( 0.8, 0.8, 0.8 );
				glVertex3d( ox, 0.0, oz );
				glVertex3d( ox, 0.0, oz+size );
				glVertex3d( ox+size, 0.0, oz+size );
				glVertex3d( ox+size, 0.0, oz );
			}
		}
	glEnd();

	glColor3f( 1.0f, 0.0f, 0.0f );
	if ( bvh )
		bvh->RenderFigure( frame_no, 0.02f );

	char  message[ 64 ];
	if ( bvh )
		sprintf( message, "%.2f (%d)", animation_time, frame_no );
	else
		sprintf( message, "Please press 'L' key to Load and display a BVH file" );
	drawMessage( 0, message );

    glutSwapBuffers();
}


void  reshape( int w, int h )
{
    glViewport(0, 0, w, h);
	
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45, (double)w/h, 1, 500 );

	win_width = w;
	win_height = h;
}


void  mouse( int button, int state, int mx, int my )
{
	if ( ( button == GLUT_LEFT_BUTTON ) && ( state == GLUT_DOWN ) )
		drag_mouse_l = 1;
	else if ( ( button == GLUT_LEFT_BUTTON ) && ( state == GLUT_UP ) )
		drag_mouse_l = 0;

	if ( ( button == GLUT_RIGHT_BUTTON ) && ( state == GLUT_DOWN ) )
		drag_mouse_r = 1;
	else if ( ( button == GLUT_RIGHT_BUTTON ) && ( state == GLUT_UP ) )
		drag_mouse_r = 0;

	if ( ( button == GLUT_MIDDLE_BUTTON ) && ( state == GLUT_DOWN ) )
		drag_mouse_m = 1;
	else if ( ( button == GLUT_MIDDLE_BUTTON ) && ( state == GLUT_UP ) )
		drag_mouse_m = 0;

	glutPostRedisplay();

	last_mouse_x = mx;
	last_mouse_y = my;
}


void  motion( int mx, int my )
{
	if ( drag_mouse_r )
	{

		camera_yaw -= ( mx - last_mouse_x ) * 1.0;
		if ( camera_yaw < 0.0 )
			camera_yaw += 360.0;
		else if ( camera_yaw > 360.0 )
			camera_yaw -= 360.0;

		camera_pitch -= ( my - last_mouse_y ) * 1.0;
		if ( camera_pitch < -90.0 )
			camera_pitch = -90.0;
		else if ( camera_pitch > 90.0 )
			camera_pitch = 90.0;
	}
	if ( drag_mouse_l )
	{

		camera_distance += ( my - last_mouse_y ) * 0.2;
		if ( camera_distance < 2.0 )
			camera_distance = 2.0;
	}

	last_mouse_x = mx;
	last_mouse_y = my;

	glutPostRedisplay();
}


void  keyboard( unsigned char key, int mx, int my )
{
	if ( key == 's' )
		on_animation = !on_animation;

	if ( ( key == 'n' ) && !on_animation )
	{
		animation_time += bvh->GetInterval();
		frame_no ++;
		frame_no = frame_no % bvh->GetNumFrame();
	}

	if ( ( key == 'p' ) && !on_animation && ( frame_no > 0 ) && bvh )
	{
		animation_time -= bvh->GetInterval();
		frame_no --;
		frame_no = frame_no % bvh->GetNumFrame();
	}

	if ( key == 'r' )
	{
		animation_time = 0.0f;
		frame_no = 0;
	}

	if ( key == 'l' )
	{
#ifdef  WIN32
		const int  file_name_len = 256;
		char  file_name[ file_name_len ] = "";

		OPENFILENAME	open_file;
		memset( &open_file, 0, sizeof(OPENFILENAME) );
		open_file.lStructSize = sizeof(OPENFILENAME);
		open_file.hwndOwner = NULL;
		open_file.lpstrFilter = "BVH Motion Data (*.bvh)\0*.bvh\0All (*.*)\0*.*\0";
		open_file.nFilterIndex = 1;
		open_file.lpstrFile = file_name;
		open_file.nMaxFile = file_name_len;
		open_file.lpstrTitle = "Select a BVH file";
		open_file.lpstrDefExt = "bvh";
		open_file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

		BOOL  ret = GetOpenFileName( &open_file );

		if( ret )
		{
			if ( bvh )
				delete  bvh;
			bvh = new BVH( file_name );

			if ( !bvh->IsLoadSuccess() )
			{
				delete  bvh;
				bvh = NULL;
			}

			animation_time = 0.0f;
			frame_no = 0;
		}
#endif
	}

	glutPostRedisplay();
}


void  idle( void )
{
	if ( on_animation )
	{
#ifdef  WIN32
		static DWORD  last_time = 0;
		DWORD  curr_time = timeGetTime();
		float  delta = ( curr_time - last_time ) * 0.001f;
		if ( delta > 0.03f )
			delta = 0.03f;
		last_time = curr_time;
		animation_time += delta;
#else
		animation_time += 0.03f;
#endif
		if ( bvh )
		{
			frame_no = animation_time / bvh->GetInterval();
			frame_no = frame_no % bvh->GetNumFrame();
		}
		else
			frame_no = 0;

		glutPostRedisplay();
	}
}


void  initEnvironment( void )
{
	float  light0_position[] = { 10.0, 10.0, 10.0, 1.0 };
	float  light0_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	float  light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	float  light0_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	glLightfv( GL_LIGHT0, GL_POSITION, light0_position );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, light0_diffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, light0_specular );
	glLightfv( GL_LIGHT0, GL_AMBIENT, light0_ambient );
	glEnable( GL_LIGHT0 );

	glEnable( GL_LIGHTING );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_DEPTH_TEST );

	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );

	glClearColor( 0.5, 0.5, 0.8, 0.0 );

}


int  main( int argc, char ** argv )
{
#ifdef  WIN32
	locale::global( locale("chinese") );
#endif

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL );
	glutInitWindowSize( 1280, 720 );
	glutInitWindowPosition( 0, 0 );
    glutCreateWindow("BVH Player Sample");
	
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
	glutMouseFunc( mouse );
	glutMotionFunc( motion );
	glutKeyboardFunc( keyboard );
	glutIdleFunc( idle );

	initEnvironment();

    glutMainLoop();
    return 0;
}

