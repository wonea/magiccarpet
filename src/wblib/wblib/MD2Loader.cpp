#include "MD2Loader.h"

/**
 * Loads a md2 and its texture
 */
MD2Loader::MD2Loader(const std::string _md2FileName,
         const std::string _textureFileName) 
{
  //std::cout << "md2 Start Processing ..." << std::endl;

  buffer = NULL;
  vertices = NULL;
	this->CleanUp();
  good = true;

  //std::cout << "md2 PerformFileHandling ..." << std::endl;
  if (0 != PerformFileHandling(_md2FileName, _textureFileName)) {
    //std::cout << "md2 PerformFileHandling failed" << std::endl;
    good = false;
    return;
  }
  //std::cout << "md2 PerformFileHandling done" << std::endl;

  //std::cout << "md2 Parse() ..." << std::endl;
	try {
		Parse();
	} catch (char* e) {
		// "EE" is signaling an exception so the user can 
		// find it more easily on the console
		std::cout << "EE " << __FUNCTION__ << " " << __FILE__ <<
			" " << __LINE__ << " " << e << std::endl;
	}
  //std::cout << "md2 Parse() done" << std::endl;

  currentAnimType = STAND;
  currentFrame = 0;
  nextFrame = 0;
  interpol = 0.0f;

  //std::cout << "md2 Processing done" << std::endl;
}

/**
 * Opens the file, loads content into buffer
 *
 * returns -1 in case of error, returns 0 in case of success
 */
int MD2Loader::PerformFileHandling(const std::string _md2FileName,
  const std::string _textureFileName) 
{
	buffer = NULL;
  textureObjectID = -1;

	// std::ios::ate - set the initial position to the end of the file
	std::ifstream file;
	file.open(_md2FileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		return -1;
	}
	int size = file.tellg();
	file.seekg (0, std::ios::beg);

	buffer = new char[size];
	file.read(buffer, size);

	if (!file.good()) {
		delete[] buffer;
		buffer = NULL;
    file.close();
    return -1;
	}

	file.close();

  textureObjectID = PCXLoader::LoadPCXTexture(_textureFileName.c_str());

  if (!glIsTexture(textureObjectID)) {
    std::cout << "md2 texture loading failed: " 
      << _textureFileName << std::endl;
    return -1;
  }

  return 0;
}

/**
 * Loading vertices:
 *
 * The file is loaded into a buffer in one piece
 * A second buffer for all vertices (three floats per vertex) is
 * allocated. The vertices from the md2 file are decompressed into
 * that vertex buffer. At decompression y and z is swapped and z
 * is negated (= swizzling). The compressed vertices are found 
 * at ofs_frames in the buffer that holds the file. At that offset,
 * frame structs are stored. Each frame contains an array of 
 * num_xyz vertex structures, which can be accessed using the 
 * verts element of the frame struct. A loop does the decompression
 * for all the vertices of each frame and stores the decompressed
 * data into the decompressed vertex buffer.
 *
 * DrawVertices() draws a frame's vertices from the decompressed vertex
 * buffer.
 *
 * Loading the mesh (triangle info):
 * Just set a triangle pointer in the buffer at offset ofs_tris.
 * This allows you to iterate over num_tris and to combine the vertices
 * of the frame you draw. The triangle information is only stored once and
 * can be used for every frame.
 * 
 * DrawMesh() uses the mesh information to render the triangles of the model
 *
 * Loading the mesh (glCmds):
 * I guess back in the days there where graphics adapters which were able
 * to execute GL_TRIANGLE_FAN and GL_TRIANGLE_STRIP and there were adapters
 * which were not capable of doing so. So there is a way to draw a md2 model
 * using GL_TRIANGLE_FAN and GL_TRIANGLE_STRIP. If you use that method, you
 * dont need the triangle information. The information which vertices to draw
 * next in the strip or fan, is stored along with the commands at 
 * ofs_glcmds. Setting a int pointer there, enables you to draw the model
 * without triangle information using glCmd information.
 *
 * Lighting:
 * two ways possible: 1. OpenGL normals and lights. If you decide this way,
 * you can either compute normals yourself or load normals from a predefined
 * table of normals called anorms.
 *
 * 2. Simulate lights using color (Quake 2 speed up trick).
 *
 *
 * Texturing:
 *
 *
 *
 * Animation:
 */
void MD2Loader::Parse() {
	if (NULL == this->buffer) { return; }

	// Parse the header
	header = (MD2Header *)buffer;

	// check format and version
	if ( (*(((char *)header) + 0) != 'I')
    || (*(((char *)header) + 1) != 'D')
    || (*(((char *)header) + 2) != 'P')
    || (*(((char *)header) + 3) != '2')
		|| (header->version != 8)) 
	{
	  CleanUp();
		throw "wrong format!";
	} //else {
	//	// "II" is signaling noncritical information
	//	std::cout << "II [md2] magic number: " << header->ident << std::endl;
	//	std::cout << "II [md2] version number: " << header->version << std::endl;
 //   std::cout << "II [md2] num_xyz: " << header->num_xyz << std::endl;
 //   std::cout << "II [md2] num_frames: " << header->num_frames << std::endl;
 //   std::cout << "II [md2] framesize: " << header->framesize << std::endl;
	//}
	
  // allocate one float[3] for each vertex in every frame
  vertices = new vec3[ header->num_xyz * header->num_frames ];

  // vertex array initialization
  vec3 * ptrverts;
  frame * framePtr;
  for( int j = 0; j < header->num_frames; j++ )
  {
      // adjust pointers
      framePtr    = (frame *)&buffer[ header->ofs_frames + header->framesize * j ];
      ptrverts    = &vertices[ header->num_xyz * j ];
      //ptrnormals  = &m_lightnormals[ num_xyz * j ];

      for( int i = 0; i < header->num_xyz; i++ )
      {
          //// swizzle y and z
          //ptrverts[i][0] = (framePtr->verts[i].v[0] * framePtr->scale[0]) + framePtr->translate[0];
          //ptrverts[i][2] = (framePtr->verts[i].v[1] * framePtr->scale[1]) + framePtr->translate[1];
          //ptrverts[i][1] = (framePtr->verts[i].v[2] * framePtr->scale[2]) + framePtr->translate[2];
          //// negate z
          //ptrverts[i][2] = -1.0f*ptrverts[i][2];
          ////ptrnormals[i] = frame->verts[i].lightnormalindex;

          // do not swizzle y and z
          // If the vertices are swizzled to OpenGL, the the mesh indices need to 
          // be swizzled too. But as this loader does not allocate extra memory
          // for the mesh indices, he cannot swizzle the mesh indices. So the loader
          // has to let the vertices unswizzled, as the mesh indices are unswizzled!
          ptrverts[i][0] = (framePtr->verts[i].v[0] * framePtr->scale[0]) + framePtr->translate[0];
          ptrverts[i][1] = (framePtr->verts[i].v[1] * framePtr->scale[1]) + framePtr->translate[1];
          ptrverts[i][2] = (framePtr->verts[i].v[2] * framePtr->scale[2]) + framePtr->translate[2];
          // do not negate z
          //ptrverts[i][2] = -1.0f*ptrverts[i][2];
          //ptrnormals[i] = frame->verts[i].lightnormalindex;
      }
  }

  // mesh pointer initialization - do not swizzle
  mesh = (triangle *)&buffer[header->ofs_tris];

  // pointer on glcmds
  glcmds = (int *)&buffer[header->ofs_glcmds];

	// set pointer to texture coordinates
  texCoords = (texCoord *)&buffer[header->ofs_st];

	//std::cout << "II [md2] loading succeeded" << std::endl;
}

void MD2Loader::DrawVertices() {

  int k = 0;

  glPushMatrix();
  glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
  glScalef(1.0f/64.0f, 1.0f/64.0f, 1.0f/64.0f);

  glBegin( GL_POINTS );

  //for (int k = 0; k < header->num_frames; k++) {
  
  // draw each triangle
  for( int i = 0; i < header->num_xyz; i++ )
  {
      //// draw triangle #i
      //for( int j = 0; j < 3; j++ )
      //{
      //    // k is the frame to draw
      //    // i is the current triangle of the frame
      //    // j is the current vertex of the triangle

          /*glTexCoord2f( (float)TexCoord[ Meshes[i].index_st[j] ].s / header->skinwidth,
                        (float)TexCoord[ Meshes[i].index_st[j] ].t / header->skinheight );

          glNormal3fv( anorms[ Vertices[ Meshes[i].index_xyz[j] ].lightnormalindex ] );
        */
    glVertex3f( vertices[i + header->num_xyz*k][0],
                      vertices[i + header->num_xyz*k][1],
                      vertices[i + header->num_xyz*k][2] );

    /*std::cout << "x: " << vertices[i + header->num_xyz*k][0]
    << " y: " << vertices[i + header->num_xyz*k][1]
    << " z: " << vertices[i + header->num_xyz*k][2]
    << std::endl;*/
      //}
  }
  //}
  glEnd();

   glPopMatrix();
}

/**
 * Draws the vertices and combines them to triangles using the triangle
 * data stored in the md2 file
 */
void MD2Loader::DrawMesh() {

  //int k = 0;

  //glFrontFace( GL_CW );

  if (glIsTexture(textureObjectID)) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureObjectID);
  }

  glPushMatrix();
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
  glScalef(1.0f/64.0f, 1.0f/64.0f, 1.0f/64.0f);

  glBegin( GL_TRIANGLES );
  // draw each triangle
  for( int i = 0; i < header->num_tris; i++ )
  {
      // draw triangle #i
      //for( int j = 0; j < 3; j++ )
      //{
          // k is the frame to draw
          // i is the current triangle of the frame
          // j is the current vertex of the triangle

          /*glTexCoord2f( (float)TexCoord[ mesh[i].index_st[j] ].s / header->skinwidth,
                        (float)TexCoord[ mesh[i].index_st[j] ].t / header->skinheight );

          glNormal3fv( anorms[ Vertices[ mesh[i].index_xyz[j] ].lightnormalindex ] );*/
        
          //glVertex3f( vertices[ mesh[i].index_xyz[j] + k*header->num_xyz ][0],
          //            vertices[ mesh[i].index_xyz[j] + k*header->num_xyz ][1],
          //            vertices[ mesh[i].index_xyz[j] + k*header->num_xyz ][2] );
      //}
      
      /*CalculateNormal(vertices[ mesh[i].index_xyz[0] + k*header->num_xyz ],
                      vertices[ mesh[i].index_xyz[1] + k*header->num_xyz ],
                      vertices[ mesh[i].index_xyz[2] + k*header->num_xyz ]);
      glVertex3fv( vertices[ mesh[i].index_xyz[0] + k*header->num_xyz ]);
      glVertex3fv( vertices[ mesh[i].index_xyz[1] + k*header->num_xyz ]);
      glVertex3fv( vertices[ mesh[i].index_xyz[2] + k*header->num_xyz ]);*/
      
    

      // do swizzling for vertices, mesh and texture coordinates here!
      CalculateNormal(vertices[mesh[i].index_xyz[0]],
                      vertices[mesh[i].index_xyz[2]],
                      vertices[mesh[i].index_xyz[1]]);

      glTexCoord2f((float)texCoords[mesh[i].index_st[0]].s / header->skinwidth,
      (float)texCoords[mesh[i].index_st[0]].t / header->skinheight );
      glVertex3fv(vertices[mesh[i].index_xyz[0]]);

      glTexCoord2f((float)texCoords[mesh[i].index_st[2]].s / header->skinwidth,
      (float)texCoords[mesh[i].index_st[2]].t / header->skinheight );
      glVertex3fv(vertices[mesh[i].index_xyz[2]]);

      glTexCoord2f((float)texCoords[mesh[i].index_st[1]].s / header->skinwidth,
      (float)texCoords[mesh[i].index_st[1]].t / header->skinheight );
      glVertex3fv(vertices[mesh[i].index_xyz[1]]);
  }
  glEnd();
  glPopMatrix();

  //glFrontFace( GL_CCW );

  glDisable(GL_TEXTURE_2D);
}

void MD2Loader::CalculateNormal(float * p1, float * p2, float * p3) {
  float a[3], b[3], result[3];
  float length;

  a[0] = p1[0] - p2[0];
  a[1] = p1[1] - p2[1];
  a[2] = p1[2] - p2[2];

  b[0] = p1[0] - p3[0];
  b[1] = p1[1] - p3[1];
  b[2] = p1[2] - p3[2];

  result[0] = a[1] * b[2] - b[1] * a[2];
  result[1] = b[0] * a[2] - a[0] * b[2];
  result[2] = a[0] * b[1] - b[0] * a[1];

  length = (float)::sqrt(result[0]*result[0] +
    result[1]*result[1] +
    result[2]*result[2]);

  glNormal3f(result[0]/length, 
    result[1]/length, 
    result[2]/length);
}

void MD2Loader::DrawFrame()
{
    //static vec3_t    vertlist[ MAX_MD2_VERTS ];  // interpolated vertices
    int              *ptricmds = glcmds;       // pointer on gl commands


    glPushMatrix();
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(1.0f/64.0f, 1.0f/64.0f, 1.0f/64.0f);


    // reverse the orientation of front-facing
    // polygons because gl command list's triangles
    // have clockwise winding
    glPushAttrib( GL_POLYGON_BIT );
    glFrontFace( GL_CW );

    // enable backface culling
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );


    // process lighting
    //ProcessLighting();

    // interpolate
    //Interpolate( vertlist );

    // bind model's texture
    //glBindTexture( GL_TEXTURE_2D, m_texid );


    // draw each triangle!
    while( int i = *(ptricmds++) )
    {
        if( i < 0 )
        {
            glBegin( GL_TRIANGLE_FAN );
            i = -i;
        }
        else
        {
            glBegin( GL_TRIANGLE_STRIP );
        }


        for( /* nothing */; i > 0; i--, ptricmds += 3 )
        {
            // ptricmds[0] : texture coordinate s
            // ptricmds[1] : texture coordinate t
            // ptricmds[2] : vertex index to render

            //float l = shadedots[ m_lightnormals[ ptricmds[2] ] ];

            // set the lighting color
            //glColor3f( l * lcolor[0], l * lcolor[1], l * lcolor[2] );

            // parse texture coordinates
            //glTexCoord2f( ((float *)ptricmds)[0], ((float *)ptricmds)[1] );

            // parse triangle's normal (for the lighting)
            // >>> only needed if using OpenGL lighting
            //glNormal3fv( anorms[ m_lightnormals[ ptricmds[2] ] ] );

            // draw the vertex
            glVertex3fv( vertices[ ptricmds[2] ] );
        }

        glEnd();
    }

    glDisable( GL_CULL_FACE );
    glPopAttrib();

    glPopMatrix();
}

void MD2Loader::DrawCurrentAnimation() {

  anim_t currentAnim = MD2Loader::animlist[currentAnimType];
  float percent = 0.2f;
  vec3 * vertsCurrFrame = NULL;
  vec3 * vertsNextFrame = NULL;
  float x1, y1, z1, x2, y2, z2;
  vec3 interpolated[3];

  if (currentAnim.first_frame > currentFrame) {
    currentFrame = currentAnim.first_frame;
  }

  if (interpol >= 1.0) {
    interpol = 0.0f;
    currentFrame++;
    if (currentFrame >= currentAnim.last_frame) {
      currentFrame = currentAnim.first_frame;
    }
    nextFrame = currentFrame + 1;
    if (nextFrame >= currentAnim.last_frame) {
      nextFrame = currentAnim.first_frame;
    }
  }

  vertsCurrFrame = (vec3 *)&vertices[header->num_xyz*currentFrame];
  vertsNextFrame = (vec3 *)&vertices[header->num_xyz*nextFrame];

  if (glIsTexture(textureObjectID)) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureObjectID);
  }

  glPushMatrix();
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
  glScalef(1.0f/64.0f, 1.0f/64.0f, 1.0f/64.0f);

  glBegin(GL_TRIANGLES);
  for (int i = 0; i < header->num_tris; i++) {
    // point 0
    x1 = vertsCurrFrame[mesh[i].index_xyz[0]][0];
    y1 = vertsCurrFrame[mesh[i].index_xyz[0]][1];
    z1 = vertsCurrFrame[mesh[i].index_xyz[0]][2];
    x2 = vertsNextFrame[mesh[i].index_xyz[0]][0];
    y2 = vertsNextFrame[mesh[i].index_xyz[0]][1];
    z2 = vertsNextFrame[mesh[i].index_xyz[0]][2];
    interpolated[0][0] = x1 + interpol * (x2 - x1);
    interpolated[0][1] = y1 + interpol * (y2 - y1);
    interpolated[0][2] = z1 + interpol * (z2 - z1);
    // point 1
    x1 = vertsCurrFrame[mesh[i].index_xyz[2]][0];
    y1 = vertsCurrFrame[mesh[i].index_xyz[2]][1];
    z1 = vertsCurrFrame[mesh[i].index_xyz[2]][2];
    x2 = vertsNextFrame[mesh[i].index_xyz[2]][0];
    y2 = vertsNextFrame[mesh[i].index_xyz[2]][1];
    z2 = vertsNextFrame[mesh[i].index_xyz[2]][2];
    interpolated[2][0] = x1 + interpol * (x2 - x1);
    interpolated[2][1] = y1 + interpol * (y2 - y1);
    interpolated[2][2] = z1 + interpol * (z2 - z1);
    // point 2
    x1 = vertsCurrFrame[mesh[i].index_xyz[1]][0];
    y1 = vertsCurrFrame[mesh[i].index_xyz[1]][1];
    z1 = vertsCurrFrame[mesh[i].index_xyz[1]][2];
    x2 = vertsNextFrame[mesh[i].index_xyz[1]][0];
    y2 = vertsNextFrame[mesh[i].index_xyz[1]][1];
    z2 = vertsNextFrame[mesh[i].index_xyz[1]][2];
    interpolated[1][0] = x1 + interpol * (x2 - x1);
    interpolated[1][1] = y1 + interpol * (y2 - y1);
    interpolated[1][2] = z1 + interpol * (z2 - z1);

    CalculateNormal(interpolated[0], interpolated[2], interpolated[1]);

    glTexCoord2f(
      (float)texCoords[mesh[i].index_st[0]].s / header->skinwidth,
      (float)texCoords[mesh[i].index_st[0]].t / header->skinheight 
      );
    glVertex3fv(interpolated[0]);
    glTexCoord2f(
      (float)texCoords[mesh[i].index_st[2]].s / header->skinwidth,
      (float)texCoords[mesh[i].index_st[2]].t / header->skinheight 
      );
    glVertex3fv(interpolated[2]);
    glTexCoord2f(
      (float)texCoords[mesh[i].index_st[1]].s / header->skinwidth,
      (float)texCoords[mesh[i].index_st[1]].t / header->skinheight 
      );
    glVertex3fv(interpolated[1]);

  }
  glEnd();

  glPopMatrix();

  interpol += percent;

  glDisable(GL_TEXTURE_2D);
}

void MD2Loader::CleanUp() {

  //std::cout << "CleanUp() MD2Loader data structures ..." << std::endl;

  //std::cout << "Start deleting buffer ..." << std::endl;
	if (NULL != this->buffer) {
		delete[] this->buffer;
		this->buffer = NULL;
	}
  //std::cout << "buffer deleted" << std::endl;

  if (NULL != vertices) {
    delete [] vertices;
    vertices = NULL;
  }

  good = true;

  //std::cout << "MD2Loader data structures cleaned" << std::endl;
}



/*void Bezier::render() {
  //std::vector<TVertex>::iterator it;
  //glBegin(GL_POINTS);
  //for (it = vertex.begin(); it != vertex.end(); it++) {
  //  glVertex3f((*it).mPosition[0], (*it).mPosition[1], (*it).mPosition[2]);
  //}
  //glEnd();

  
  //
  //// Draw both rows! Danger: This only suffices if you have 2 rows!
  //glBegin(GL_TRIANGLE_STRIP);
  //for (int i = 0; i < 2*3; i++) {
  //  glVertex3f(vertex[indexes[i]].mPosition[0], vertex[indexes[i]].mPosition[1], vertex[indexes[i]].mPosition[2]);
  //}
  //glEnd();

  //glBegin(GL_TRIANGLE_STRIP);
  //for (int i = 2*3; i < 2*2*3; i++) {
  //  glVertex3f(vertex[indexes[i]].mPosition[0], vertex[indexes[i]].mPosition[1], vertex[indexes[i]].mPosition[2]);
  //}
  //glEnd();

  //std::cout << "ERROR: Bezier::render()!" << std::endl;
  //glVertexPointer(3, GL_FLOAT, sizeof(TVertex), &vertex[0].position);
  //glVertexPointer(3, GL_FLOAT, sizeof(TVertex), &vertex[0].mPosition);

  //glClientActiveTextureARB(GL_TEXTURE0_ARB);
  //glTexCoordPointer(2, GL_FLOAT, sizeof(TVertex), &vertex[0].textureCoord);

  //glClientActiveTextureARB(GL_TEXTURE1_ARB);
  //glTexCoordPointer(2, GL_FLOAT, sizeof(TVertex), &vertex[0].lightmapCoord);

  //glMultiDrawElementsEXT(GL_TRIANGLE_STRIP, trianglesPerRow.getCArray(),
  //    GL_UNSIGNED_INT, (const void **)(rowIndexes.getCArray()), level);
  //glMultiDrawElementsEXT(GL_TRIANGLE_STRIP, trianglesPerRow.,
  //    GL_UNSIGNED_INT, (const void **)(rowIndexes), level);

  glEnableClientState(GL_VERTEX_ARRAY);

  glVertexPointer(3, // every vertex consists of 3 coordinates 
    GL_FLOAT, // each coordinate is of type float
    //sizeof(T3DSVertex), 
    //sizeof(ThreeDSObject),
    sizeof(TVertex),
    //this->vertexArray[0].mPosition);  
    vertex.front().mPosition);

  //void glMultiDrawElementsEXT(GLenum  mode,
  //  GLsizei  *count, // Points to an array of the element counts
  //  GLenum  type, // type of values in 4th parameter
  //  const GLvoid  **indices, // Specifies a pointer to the location where the indices are stored.
  //  GLsizei  primcount) // size of count
  //glMultiDrawElementsEXT(GL_TRIANGLE_STRIP,
  //  (GLsizei *)trianglesPerRow.front(),
  //  GL_UNSIGNED_INT,
  //  (const void **)(rowIndexes.front()),
  //  (GLsizei)level);

  for (int i = 0; i < (GLsizei)level; ++i)
  {
   if (trianglesPerRow[i] > 0)
   {
    //glDrawElements(GL_TRIANGLE_STRIP, trianglesPerRow.front(), GL_UNSIGNED_INT, (const void **)(rowIndexes.front()));
    glDrawElements(GL_TRIANGLE_STRIP, trianglesPerRow[i], GL_UNSIGNED_INT, (const void **)(rowIndexes[i]));
   }
  }

  glDisableClientState(GL_VERTEX_ARRAY);


 // glDisableClientState(GL_NORMAL_ARRAY);
 // //glEnableClientState(GL_NORMAL_ARRAY);

	//glDisableClientState(GL_INDEX_ARRAY);
 // //glEnableClientState(GL_INDEX_ARRAY);

 // //glDisableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_VERTEX_ARRAY);

 // glVertexPointer(3, // every vertex consists of 3 coordinates 
 //   GL_FLOAT, // each coordinate is of type float
 //   //sizeof(T3DSVertex), 
 //   //sizeof(ThreeDSObject),
 //   sizeof(TVertex),
 //   //this->vertexArray[0].mPosition);  
 //   vertex.front().mPosition);
 // 
 // //glIndexPointer(GL_UNSIGNED_SHORT, 0, this->faceIndex);
 //   
 // //glPushMatrix();
 //   //glTranslatef(-pos[0], -pos[1], -pos[2]);
 //   
 //   
 //   //glBegin(GL_TRIANGLES);

 //   // void glDrawElements( GLenum mode,
	//	//       GLsizei count,
	//	//       GLenum type,
	//	//       const GLvoid *indices )			       
 //   glDrawElements(
 //     GL_TRIANGLE_STRIP, 
 //     level*level*2, 
 //     GL_FLOAT, 
 //     //Objects[i].MatFaces[j].subFaces);
 //     //this->faceArray);
 //     this->rowIndexes.front());

 //   //glEnd();  
 // //glPopMatrix();

 // 

 // glDisableClientState(GL_NORMAL_ARRAY);
 // //glEnableClientState(GL_NORMAL_ARRAY);

	//glDisableClientState(GL_INDEX_ARRAY);
 // //glEnableClientState(GL_INDEX_ARRAY);

 // glDisableClientState(GL_VERTEX_ARRAY);
	////glEnableClientState(GL_VERTEX_ARRAY);
}*/




/*void Q3BSP::DrawWithoutPVS() {

  //glDisable(GL_LIGHTING);
  //glDisable(GL_COLOR_MATERIAL);
  
	glVertexPointer(3, GL_FLOAT, sizeof(TVertex), vertices[0].mPosition);
  //glNormalPointer(GL_FLOAT, sizeof(TVertex), vertices[0].mNormal);  
	//// Since we are using vertex arrays, we need to tell OpenGL which texture
	//// coordinates to use for each texture pass.  We switch our current texture
	//// to the first one, then set our texture coordinates.
	//glTexCoordPointer(2, GL_FLOAT, sizeof(TVertex), vertices[0].mTexcoord);

	//// Set our vertex array client states for vertices and texture coordinates
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  
  glFrontFace(GL_CW);
	for (int i = 0; i < faceCount; i++) {
	
	  //if (i != currentFace) {
	  //  continue;
	  //}
	  
	  if (!glIsTexture(texIDtexObjMap[faces[i].mTexture])) {
	    std::cout << "This is not a valid texture object !" << std::endl;
	  } else {	  
	    glActiveTextureARB(GL_TEXTURE0_ARB + 0); 
      glBindTexture(GL_TEXTURE_2D, texIDtexObjMap[faces[i].mTexture]);
      //glDisable(GL_TEXTURE_2D);
      glEnable(GL_TEXTURE_2D);
    }
  
		// Before drawing this face, make sure it's a normal polygon or a mesh
		if ((FACE_TYPE_POLYGON == faces[i].mType) || (FACE_TYPE_MESH == faces[i].mType)) {
			glDrawArrays(GL_TRIANGLE_FAN, faces[i].mVertex, faces[i].m_n_vertexes);
			//glDrawArrays(GL_TRIANGLES, faces[i].mVertex, faces[i].m_n_vertexes);
		}
	}
	glFrontFace(GL_CCW);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	//glDisable(GL_TEXTURE_2D);
}*/

//void Q3BSP::DrawVertices() {
//  glBegin(GL_POINTS);
//	for (int i = 0; i < vertexCount; i++) {
//	  glVertex3fv(this->vertices[i].mPosition);
//	}
//	glEnd();
//}

//void Q3BSP::Draw(float camPos[3]) {
//  
//  int i;
//  int currentLeafIndex; // This variable stores the leave the camera is in
//      
//  // Find the leave the camera is in
//  currentLeafIndex = GetCurrentLeafIndex(camPos);
//  
//  // Reset allreadyVisibleList
//  ResetAlreadyVisible();
//   
//  // Find other leaves that are potentially visible
//  for (i = 0; i < this->leafCount; i++) {
//    if (IsPotentiallyVisible(this->leaves[currentLeafIndex].mCluster, 
//                             this->leaves[i].mCluster) ) {
//      RenderLeaf(i);                   
//    }
//  }
//}

/*void Q3BSP::Draw(float camera_x, float camera_y, float camera_z) {
  
  int i;
  int currentLeafIndex; // This variable stores the leave the camera is in
  float camPos[3] = {camera_x, camera_y, camera_z};
      
  // Find the leave the camera is in
  currentLeafIndex = GetCurrentLeafIndex(camPos);
  //currentLeafIndex = 0;
  
  // Reset alreadyVisibleList
  ResetAlreadyVisible();
   
  // Somehow Quake3 specifies faces clockwise when frontfacing
  glFrontFace(GL_CW);

  // Find other leaves that are potentially visible
  for (i = 0; i < this->leafCount; i++) {
    if (IsPotentiallyVisible(this->leaves[currentLeafIndex].mCluster, 
      this->leaves[i].mCluster)) 
    {
      //std::cout << i << std::endl;
      RenderLeaf(i);                   
    }
  }
  
  //RenderLeaf(21);
  
  //RenderCluster(currentCluster);

  // the rest of the world does not ...
  glFrontFace(GL_CCW);
  
 // glVertexPointer(3, GL_FLOAT, sizeof(TVertex), vertices[0].mPosition);
 // glEnableClientState(GL_VERTEX_ARRAY);
 // for (int i = 0; i < faceCount; i++) {
	//	// Before drawing this face, make sure it's a normal polygon or a mesh
	//	if ((FACE_TYPE_POLYGON == faces[i].mType) || (FACE_TYPE_MESH == faces[i].mType)) {
	//		glDrawArrays(GL_TRIANGLE_FAN, faces[i].mVertex, faces[i].m_n_vertexes);
	//	}
	//}
}*/

/*void Q3BSP::RenderPolygonFace(TFace _f) {
  int stride;
  int offset;
  
  stride = sizeof(TVertex); // determine the size of one BSP Vertex structure
  offset = _f.mVertex;
  
  // Vertex and normal arrays work independently of the texture unit
  // (only texture coord arrays need to be set up for each activeted
  // texture unit seperately)
  
  //glDisableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, stride, &vertices[offset].mNormal);
  
	//glDisableClientState(GL_INDEX_ARRAY);
  //glEnableClientState(GL_INDEX_ARRAY);

  //glDisableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, &vertices[offset].mPosition);
	
	
  
  // set texture pointer and enable texture coord state for texture unit 0
  glClientActiveTextureARB(GL_TEXTURE0_ARB);
	
	if (glIsTexture(texIDtexObjMap[_f.mTexture])) {
	  glEnableClientState(GL_TEXTURE_COORD_ARRAY);	
	  glTexCoordPointer(2, GL_FLOAT, stride, &vertices[offset].mTexcoord);
    //glClientActiveTextureARB(GL_TEXTURE0_ARB);
    //glTexCoordPointer(2, GL_FLOAT, stride, &(vertex[offset].textureCoord));
    //glClientActiveTextureARB(GL_TEXTURE1_ARB);
    //glTexCoordPointer(2, GL_FLOAT, stride, &(vertex[offset].lightmapCoord));
    //glActiveTextureARB(GL_TEXTURE0_ARB + 0); 
    glBindTexture(GL_TEXTURE_2D, texIDtexObjMap[_f.mTexture]);
    //glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
  } else {
    glDisable(GL_TEXTURE_2D);
  }
  
  if (!nolightmaps) {
    // set texture pointer and enable texture coord state for texture unit 1
    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    if (glIsTexture(lightmapArray[_f.lm_index])) {
      // If we want to view the lightmap textures
	    //if (g_bLightmaps)
	    //{
	    // Turn on texture arrays for the second lightmap pass
	    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	    //glTexCoordPointer(2, GL_FLOAT, stride, &vertices[offset].mTexcoord[1]);
	    glTexCoordPointer(2, GL_FLOAT, stride, &vertices[offset].mLightMap);
	    //}
	    // Set the current pass as the second lightmap texture_
      glActiveTextureARB(GL_TEXTURE1_ARB);
      // Turn on texture mapping and bind the face's lightmap over the texture
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, lightmapArray[_f.lm_index]);
    }
  }
  
  glDrawElements(GL_TRIANGLES, _f.m_n_meshverts,
                 GL_UNSIGNED_INT, &(this->meshverts[_f.mMeshvert]));
  
  //glDrawArrays(GL_TRIANGLE_FAN, this->meshverts[_f.mMeshvert].mMeshVert,
  //             _f.m_n_meshverts);

  
  glDisableClientState(GL_NORMAL_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);

	//glDisableClientState(GL_INDEX_ARRAY);
  //glEnableClientState(GL_INDEX_ARRAY);

  glDisableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_VERTEX_ARRAY);

  glActiveTextureARB(GL_TEXTURE0_ARB);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  
  glActiveTextureARB(GL_TEXTURE1_ARB);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}*/

/*void Q3BSP::RenderPolygonFaceDebug(TFace f) {
  
  //int stride;
  
  
  //stride = sizeof(Vertex);
  //glVertexPointer(3, GL_FLOAT, stride, &(map->vertexLumps[0].Position));
  //glEnableClientState(GL_VERTEX_ARRAY);
  //glDrawArrays(GL_TRIANGLE_FAN, f.FirstVertex, f.VertexCount);
  
  // determine the size of one BSP Vertex structure
  //stride = sizeof(Vertex);
  // Set the pointer on the very first Vertex in the Vertex Lump 
  //glVertexPointer(3, GL_FLOAT, stride, &(map->vertexLumps[0].Position));
  //glClientActiveTextureARB(GL_TEXTURE0_ARB);
  //glTexCoordPointer(2, GL_FLOAT, stride, &(vertex[offset].textureCoord));
  //glClientActiveTextureARB(GL_TEXTURE1_ARB);
  //glTexCoordPointer(2, GL_FLOAT, stride, &(vertex[offset].lightmapCoord));
  //glEnableClientState(GL_VERTEX_ARRAY);
  //glDrawElements(GL_TRIANGLES, f.MeshVertexCount,
  //GL_UNSIGNED_INT, &(map->meshVertLumps[f.FirstMeshVertex]));
  // Draw using direkt indizes in the vertex array 
  //glDrawArrays(GL_TRIANGLE_FAN, f.FirstVertex, f.VertexCount);
  
  glDisableClientState(GL_NORMAL_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
  //glEnableClientState(GL_INDEX_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  
  glActiveTextureARB(GL_TEXTURE0_ARB + 0); 
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  //glEnable(GL_TEXTURE_2D);
  
  glDisable(GL_LIGHTING);
    
  glBegin(GL_TRIANGLE_FAN);
  for (int i = f.mVertex; i < (f.mVertex + f.m_n_vertexes); i++) {
    glNormal3fv(this->vertices[i].mNormal);
    glVertex3fv(this->vertices[i].mPosition);
  }
  glEnd();
  
  glEnable(GL_LIGHTING);
  
//  glVertexPointer(3, GL_FLOAT, sizeof(TVertex), this->vertices);
//  glNormalPointer(GL_FLOAT, sizeof(TVertex), this->vertices->mNormal);
//  glEnableClientState(GL_VERTEX_ARRAY);
//  glEnableClientState(GL_NORMAL_ARRAY);
//  for (int i = 0; i < this->faceCount; i++) {
//		// Before drawing this face, make sure it's a normal polygon or a mesh
////		if ((1 == map->faceLumps[i].Type) || (3 == map->faceLumps[i].Type)) {
//			glDrawArrays(GL_TRIANGLE_FAN, this->faces[i].mVertex, this->faces[i].m_n_vertexes);
//		//}
//	}
}*/

anim_t MD2Loader::animlist[ 21 ] = 
{
    // first, last, fps

    {   0,  39,  9 },   // STAND
    {  40,  45, 10 },   // RUN
    {  46,  53, 10 },   // ATTACK
    {  54,  57,  7 },   // PAIN_A
    {  58,  61,  7 },   // PAIN_B
    {  62,  65,  7 },   // PAIN_C
    {  66,  71,  7 },   // JUMP
    {  72,  83,  7 },   // FLIP
    {  84,  94,  7 },   // SALUTE
    {  95, 111, 10 },   // FALLBACK
    { 112, 122,  7 },   // WAVE
    { 123, 134,  6 },   // POINT
    { 135, 153, 10 },   // CROUCH_STAND
    { 154, 159,  7 },   // CROUCH_WALK
    { 160, 168, 10 },   // CROUCH_ATTACK
    { 169, 172,  7 },   // CROUCH_PAIN
    { 173, 177,  5 },   // CROUCH_DEATH
    { 178, 183,  7 },   // DEATH_FALLBACK
    { 184, 189,  7 },   // DEATH_FALLFORWARD
    { 190, 197,  7 },   // DEATH_FALLBACKSLOW
    { 198, 198,  5 },   // BOOM
};
