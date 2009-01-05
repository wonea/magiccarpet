#include "Q3BSP.h"

/**
 * Loads the complete file into buffer
 */
Q3BSP::Q3BSP(const std::string _fileName) {

  //std::cout << "Q3BSP Start Processing ..." << std::endl;
  
	/*header = NULL;
	faces = NULL;
	vertices = NULL;
	faceCount = 0;
	vertexCount = 0;*/
	good = true;

  buffer = NULL;
  alreadyVisible = NULL;

	lightmapArray = NULL;
	
	this->CleanUp();

  //std::cout << "Q3BSP PerformFileHandling ..." << std::endl;
  if (PerformFileHandling(_fileName)) {
    std::cout << __FILE__ << " " << __LINE__ << " Could not load " << _fileName << std::endl;
    good = false;
    return;
  }
  //std::cout << "Q3BSP PerformFileHandling done" << std::endl;

  //std::cout << "Q3BSP Parse() ..." << std::endl;
	try {
		Parse();
	} catch (char* e) {
		// "EE" is signaling an exception so the user can 
		// find it more easily on the console
		std::cout << "EE " << __FUNCTION__ << " " << __FILE__ <<
			" " << __LINE__ << " " << e << std::endl;
	}
  //std::cout << "Q3BSP Parse() done" << std::endl;
	
	/*std::cout << this->clusterCount << std::endl;
	for (int i = 0; i < this->clusterCount; i++) {
	  if (this->IsPotentiallyVisible(836, i)) {
      std::cout << 836 << " see " << i << std::endl;
    }
	}*/
	
	/*if (this->IsPotentiallyVisible(836, 727)) {
    std::cout << 836 << " see " << 727 << std::endl;
  } else {
    std::cout << 836 << " dont see " << 727 << std::endl;
  }*/
	 
	// DEBUG
	//DumpLeavesContainingFace(11);

  // Tesselate Patches
  //std::cout << "Q3BSP Tesselate() ..." << std::endl;
  TesselatePatches(2);
  //std::cout << "Q3BSP Tesselate() done" << std::endl;
  
  outputStartsOut = true;
  outputAllSolid = false;
  outputFraction = 1.0f;
  outputEnd[0] = outputEnd[1] = outputEnd[2] = 0.0f;

  //std::cout << "Q3BSP Processing done" << std::endl;
}

/**
 * Opens the file, loads content into buffer
 */
int Q3BSP::PerformFileHandling(const std::string _fileName) {
	buffer = NULL;

	// std::ios::ate - set the initial position to the end of the file
	std::ifstream file;
	file.open(_fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
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
    return -1;
	}

	file.close();

  return 0;
}

EntityParser::EntityParser(const char * _entityString) {
  char key[256];
  char value[256];
  char * buffer = key;
  int index = 0;
  const char * pch = NULL;
  EntityParserMode mode = PREKEY; 
  currentDescriptor = NULL; 
  
  //std::cout << _entityString << std::endl;
  
  pch = _entityString;
  
  while (*pch != '\0') {
    
    if (*pch == '"') {
      switch (mode) {
        case PREKEY:
          buffer = key;
          mode = KEY;
          index = -1;
          break;
          
        case KEY:
          buffer[index] = '\0';
          mode = PREVALUE;
          buffer = value;
          index = -1;
          break;
          
        case PREVALUE:
          buffer = value;
          mode = VALUE;
          index = -1;
          break;
          
        case VALUE:
          buffer[index] = '\0';
          mode = PREKEY;
          buffer = key;
          index = -1;
          
          Process(key, value);
          break;
          
        default:
          break;
      } 
    } else if (*pch == '{') {
      EntityStart();
    } else if (*pch == '}') {
      EntityEnd();
    } else {
      buffer[index] = *pch;
    }
    
    index++;
    pch++;
  }
}

/**
 * Called whenever a new entity begins '{'
 */
void EntityParser::EntityStart() {
  //std::cout << "EntityStart()" << std::endl;
  currentDescriptor = new EntityDescriptor();
  entityDescriptorList.push_back(currentDescriptor);
}

/**
 * Called whenever the current entity ends '}'
 */
void EntityParser::EntityEnd() {
  //std::cout << "EntityEnd()" << std::endl;
}

/**
 * Called whenever the parser processed one key value pair
 */
void EntityParser::Process(const char * _key, const char * _value) {

  //if ((0 == strcmp("info_player_deathmatch", _value))) {
  //  std::cout << "Key: " << _key << " Value: " << _value << std::endl;
  //}
  
  //std::cout << _value << " ";
  
  KeyValuePair * kvp = new KeyValuePair();
  memcpy(kvp->key, _key, 100);
  memcpy(kvp->value, _value, 100);
  currentDescriptor->keyValueList.push_back(kvp);
}

void EntityParser::CleanUp() {
  std::list<EntityDescriptor *>::iterator it;
	std::list<EntityDescriptor *>::iterator tempit;
	bool done = false;
	
	it = entityDescriptorList.begin();
	while (!done) {	
	  tempit = it;
	  // if it == partList.end() it doesn�t point on any object.
	  // Then it is invalid and points "behind" the last object of the list
	  if (it == entityDescriptorList.end()) {
	    done = true;
	    continue;
	  }
	  it++;
	  (*tempit)->CleanUp();
	  delete (EntityDescriptor *)(*tempit);
		entityDescriptorList.remove(*tempit);
	}
}

EntityDescriptor * EntityParser::GetDescriptorContaining(const char * _key, 
  const char * _value) 
{
  std::list<EntityDescriptor *>::iterator it;
  
  if (0 == entityDescriptorList.size()) {
    return NULL;
  }
  
  for (it = entityDescriptorList.begin(); it != entityDescriptorList.end(); it++) {
  
    std::list<KeyValuePair *>::iterator it2;
    for (it2 = (*it)->keyValueList.begin(); it2 != (*it)->keyValueList.end(); it2++) {
      const char * key = (*it2)->key;
      const char * value = (*it2)->value;
      if ((0 == strcmp(key, _key)) && (0 == strcmp(value, _value))) {
        return (*it);
      }
    }
  
  }
  
  return NULL;
}

void EntityDescriptor::CleanUp() {
  std::list<KeyValuePair *>::iterator it;
	std::list<KeyValuePair *>::iterator tempit;
	bool done = false;
	
	it = keyValueList.begin();
	while (!done) {	
	  tempit = it;
	  // if it == partList.end() it doesn�t point on any object.
	  // Then it is invalid and points "behind" the last object of the list
	  if (it == keyValueList.end()) {
	    done = true;
	    continue;
	  }
	  it++;
	  //(*tempit)->CleanUp();
	  delete (KeyValuePair *)(*tempit);
		keyValueList.remove(*tempit);
	}
}

const char * EntityDescriptor::Get(const char * _key) {
  std::list<KeyValuePair *>::iterator it;
  
  if (0 == keyValueList.size()) {
    return NULL;
  }
  
  for (it = keyValueList.begin(); it != keyValueList.end(); it++) {
    if (0 == strcmp((*it)->key, _key)) {
      return (*it)->value;
    }
  }
  
  return NULL;
}

/**
 * param _levelOfTesselation - enter a number between 2 and 10
 * two corresponds to no further tesselation. The Biquadratic
 * Bezier Patch remains unchanged. _levelOfTesselation actually
 * describes the number of edges along one of the two sides of the
 * tesselated surface. If you enter 2 edges you get 3 Vertices for
 * each side. This is already stored in the bsp file itself.
 *
 * A number larger than 2 for _levelOfTesselation produces new 
 * vertices through interpolation. You should not enter more than
 * 10 edges, as this produces huge amounts of triangles to render.
 */
void Bezier::tessellate(int _levelOfTesselation) {
    level = _levelOfTesselation;

    // The number of vertices along a side is 1 + num edges
    const int L1 = _levelOfTesselation + 1;

    vertex.resize(L1 * L1);

    // Compute the vertices
    int i;

    // Interpolate down the t direction at s = 0. This is directly
    // possible using P00, P10 and P20 which are read from
    // the BSP file
    //
    // This loop creates _levelOfTesselation + 1 many vertices
    for (i = 0; i <= _levelOfTesselation; ++i) {
        double a = (double)i / _levelOfTesselation;
        double b = 1 - a;

        for (int r = 0; r < 3; r++) {
          vertex[i].mPosition[r] =
              controls[0].mPosition[r] * (b * b) + 
              controls[3].mPosition[r] * (2 * b * a) +
              controls[6].mPosition[r] * (a * a);
        }
    }

    // Interpolate down the t direction for s positions
    // greater than 0
    for (i = 1; i <= _levelOfTesselation; ++i) {
        double a = (double)i / _levelOfTesselation;
        double b = 1.0 - a;

        // To interpolate points on a quadratic bezier curve,
        // three control points are needed. As we create a 
        // biquadtratic bezier patch, we have to interpolate
        // three temporary control points for the s direction first,
        // store them in temp and after that we use the temporary
        // control points to interpolate vertices down the t direction
        TVertex temp[3];

        int j;

        // For the i�th step made in s direction:

        // Interpolate the points on C0, C1 and C2, which
        // are used as control points, for the interpolation
        // of vertices between C0, C1 and C2 and store them in temp
        for (j = 0; j < 3; ++j) {
            int k = 3 * j;
            //temp[j] =
            //    controls[k + 0] * (b * b) + 
            //    controls[k + 1] * (2 * b * a) +
            //    controls[k + 2] * (a * a);

            for (int r = 0; r < 3; r++) {
              temp[j].mPosition[r] =
                  controls[k + 0].mPosition[r] * (b * b) + 
                  controls[k + 1].mPosition[r] * (2 * b * a) +
                  controls[k + 2].mPosition[r] * (a * a);
            }

        }

        // Use the interpolated control points stored in temp
        // to interpolate vertices down the t direction
        for(j = 0; j <= _levelOfTesselation; ++j) {
            double a = (double)j / _levelOfTesselation;
            double b = 1.0 - a;

            for (int r = 0; r < 3; r++) {
              vertex[i * L1 + j].mPosition[r] =
                  temp[0].mPosition[r] * (b * b) + 
                  temp[1].mPosition[r] * (2 * b * a) +
                  temp[2].mPosition[r] * (a * a);
            }
        }
    }


    // Compute the indices for calls to glDrawRangeElements() or
    // glDrawElements()
    int row;

    indexes.resize(_levelOfTesselation * (_levelOfTesselation + 1) * 2);

    for (row = 0; row < _levelOfTesselation; ++row) {
        for(int col = 0; col <= _levelOfTesselation; ++col)	{
            indexes[(row * (_levelOfTesselation + 1) + col) * 2 + 1] = row       * L1 + col;
            indexes[(row * (_levelOfTesselation + 1) + col) * 2]     = (row + 1) * L1 + col;
        }
    }

    /*unsigned int * ref = &indexes.front();
    for (int i = 0; i < (_levelOfTesselation * (_levelOfTesselation + 1) * 2); i++) {
      std::cout << indexes[i] << " " << *ref << std::endl;
      ref++;      
    }*/

    trianglesPerRow.resize(_levelOfTesselation);

    rowIndexes.resize(_levelOfTesselation);

    for (row = 0; row < _levelOfTesselation; ++row) {
        trianglesPerRow[row] = 2 * L1;
        rowIndexes[row]      = &indexes[row * 2 * L1];
    }
}

void Bezier::renderDebug() {
  /*std::vector<TVertex>::iterator it;
  glBegin(GL_POINTS);
  for (it = vertex.begin(); it != vertex.end(); it++) {
    glVertex3f((*it).mPosition[0], (*it).mPosition[1], (*it).mPosition[2]);
  }
  glEnd();*/

  glBegin(GL_POINTS);
  for (int i = 0; i < 9; i++) {
    glVertex3f(controls[i].mPosition[0]+(i*0.02f), controls[i].mPosition[1], controls[i].mPosition[2]);
  }
  glEnd();
}

void Bezier::render() {
  /*std::vector<TVertex>::iterator it;
  glBegin(GL_POINTS);
  for (it = vertex.begin(); it != vertex.end(); it++) {
    glVertex3f((*it).mPosition[0], (*it).mPosition[1], (*it).mPosition[2]);
  }
  glEnd();*/

  
  /*
  // Draw both rows! Danger: This only suffices if you have 2 rows!
  glBegin(GL_TRIANGLE_STRIP);
  for (int i = 0; i < 2*3; i++) {
    glVertex3f(vertex[indexes[i]].mPosition[0], vertex[indexes[i]].mPosition[1], vertex[indexes[i]].mPosition[2]);
  }
  glEnd();

  glBegin(GL_TRIANGLE_STRIP);
  for (int i = 2*3; i < 2*2*3; i++) {
    glVertex3f(vertex[indexes[i]].mPosition[0], vertex[indexes[i]].mPosition[1], vertex[indexes[i]].mPosition[2]);
  }
  glEnd();*/

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


  /*glDisableClientState(GL_NORMAL_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);

	glDisableClientState(GL_INDEX_ARRAY);
  //glEnableClientState(GL_INDEX_ARRAY);

  //glDisableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

  glVertexPointer(3, // every vertex consists of 3 coordinates 
    GL_FLOAT, // each coordinate is of type float
    //sizeof(T3DSVertex), 
    //sizeof(ThreeDSObject),
    sizeof(TVertex),
    //this->vertexArray[0].mPosition);  
    vertex.front().mPosition);
  
  //glIndexPointer(GL_UNSIGNED_SHORT, 0, this->faceIndex);
    
  //glPushMatrix();
    //glTranslatef(-pos[0], -pos[1], -pos[2]);
    
    
    //glBegin(GL_TRIANGLES);

    // void glDrawElements( GLenum mode,
		//       GLsizei count,
		//       GLenum type,
		//       const GLvoid *indices )			       
    glDrawElements(
      GL_TRIANGLE_STRIP, 
      level*level*2, 
      GL_FLOAT, 
      //Objects[i].MatFaces[j].subFaces);
      //this->faceArray);
      this->rowIndexes.front());

    //glEnd();  
  //glPopMatrix();

  

  glDisableClientState(GL_NORMAL_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);

	glDisableClientState(GL_INDEX_ARRAY);
  //glEnableClientState(GL_INDEX_ARRAY);

  glDisableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_VERTEX_ARRAY);*/
}


void Q3BSP::Parse() {
	if (NULL == this->buffer) { return; }

	// Parse the header
	header = (THeader*)buffer;

	// check format and version
	if ((header->mMagicNumber[0] != 'I') 
		|| (header->mMagicNumber[1] != 'B') 
		|| (header->mMagicNumber[2] != 'S')
		|| (header->mMagicNumber[3] != 'P')
		|| (header->mVersion != 46)) 
	{
	  CleanUp();
		throw "wrong format!";
	} //else {
		// "II" is signaling noncritical information
		//std::cout << "II [Q3BSP] magic number: " << header->mMagicNumber << std::endl;
		//std::cout << "II [Q3BSP] version number: " << header->mVersion << std::endl;
	//}

	// set pointer into the buffer onto the faces lump
	faces = (TFace*)&(this->buffer[header->mLumps[FACES].mOffset]);
	if (NULL == faces) {
	  CleanUp();
		throw "cast into faces lump failed!";
	}
	// Estimate number of faces
	faceCount = (header->mLumps[FACES].mLength / sizeof(TFace));
	//std::cout << "II [Q3BSP] face count: " << faceCount << std::endl;

	// Print the types of all the faces
	/*for (int i = 0; i < faceCount; i++) {
		std::cout << "II [Q3BSP] face number " << i;
		switch (faces[i].mType) {
			case FACE_TYPE_POLYGON:
				std::cout << " is of type POLYGON" << std::endl;
				break;
			case FACE_TYPE_PATCH:
				std::cout << " is of type PATCH" << std::endl;
				break;
			case FACE_TYPE_MESH:
				std::cout << " is of type MESH" << std::endl;
				break;
			case FACE_TYPE_BILLBOARD:
				std::cout << " is of type BILLBOARD" << std::endl;
				break;
			default:
				std::cout << " is of unknown type!" << std::endl;
				break;
		}
	}*/
	
	// initialize already visible list
  this->bytesInAlreadyVisibleList = (this->faceCount / 8) + 1;
  //this->alreadyVisible = malloc(map->BytesInAlreadyVisibleList);
  this->alreadyVisible = new unsigned char[this->bytesInAlreadyVisibleList];
  for (int i = 0; i < this->bytesInAlreadyVisibleList; i++) {
    this->alreadyVisible[i] = 0;
  }

	// set pointer into the buffer onto the vertices lump
	vertices = (TVertex*)&(this->buffer[header->mLumps[VERTEXES].mOffset]);
	if (NULL == vertices) {
	  CleanUp();
		throw "cast into vertices lump failed!";
	}
	// swizzle vertices
	vertexCount = (header->mLumps[VERTEXES].mLength / sizeof(TVertex));
	/*std::cout << "Length: " << header->mLumps[VERTEXES].mLength << std::endl;
	std::cout << "Offset: " << header->mLumps[VERTEXES].mOffset << std::endl;
	std::cout << "sizeof Vertex: " << sizeof(TVertex) << std::endl;
	std::cout << "II [Q3BSP] vertex count: " << vertexCount << std::endl;*/
	/*for (int i = 0; i < vertexCount; i++) {
		float temp = vertices[i].position[1];
		vertices[i].position[1] = vertices[i].position[2];
		vertices[i].position[2] = -temp;
	}*/
	// Change vertex postitions and vertex normals
  for (int i = 0; i < vertexCount; i++) {
    // swap y and z and negate z
    float temp = vertices[i].mPosition[1];
		vertices[i].mPosition[1] = vertices[i].mPosition[2];
		vertices[i].mPosition[2] = -temp;
    
    // scale down distances
    this->vertices[i].mPosition[0] /= SCALE_FACTOR;
    this->vertices[i].mPosition[1] /= SCALE_FACTOR;
    this->vertices[i].mPosition[2] /= SCALE_FACTOR;
    
    /*std::cout << i << "x: " << this->vertices[i].mPosition[0]
              << "y: " << this->vertices[i].mPosition[1]
              << "z: " << this->vertices[i].mPosition[2] << std::endl;*/
    
    // swap y and z and negate z
    temp = this->vertices[i].mNormal[1];
    this->vertices[i].mNormal[1] = this->vertices[i].mNormal[2];
    this->vertices[i].mNormal[2] = -temp;
  }
	
	// set pointer into the buffer onto the entitiy lump
	entities = (TEntity*)&(this->buffer[header->mLumps[ENTITIES].mOffset]);
	if (NULL == entities) {
	  CleanUp();
		throw "cast into entities lump failed!";
	}
	EntityParser * ep =  new EntityParser((char*)entities);
	//std::cout << ep->entityDescriptorList.size() << " entities parsed" << std::endl;
	EntityDescriptor * ed = ep->GetDescriptorContaining("classname", "info_player_deathmatch");
	if (NULL != ed) {
	  //std::cout << "Retrieved ed" << std::endl;
	  const char * origin = ed->Get("origin");
	  ParseVector(origin, info_player_start);
	} else {
	  EntityDescriptor * ed = ep->GetDescriptorContaining("classname", "info_player_start");
	  //std::cout << "Retrieved ed" << std::endl;
	  const char * origin = ed->Get("origin");
	  ParseVector(origin, info_player_start);
	}
  /*std::cout << (float)info_player_start[0] << " | " 
    << (float)info_player_start[1] << " | " 
    << (float)info_player_start[2]
    << std::endl;*/
    
  // swizzle and downscale
  float temp = info_player_start[1];
  info_player_start[1] = info_player_start[2];
  info_player_start[2] = -temp;
  info_player_start[0] /= SCALE_FACTOR;
  info_player_start[1] /= SCALE_FACTOR;
  info_player_start[2] /= SCALE_FACTOR;
	
	ep->CleanUp();
	ep = NULL;
	
	// set pointer into the buffer onto the texture lump
	textures = (TTexture*)&(this->buffer[header->mLumps[TEXTURES].mOffset]);
	if (NULL == textures) {
	  CleanUp();
		throw "cast into textures lump failed!";
	}
	int texturesLumpCount = this->header->mLumps[TEXTURES].mLength / sizeof(TTexture);
	for (int i = 0; i < texturesLumpCount; i++) {
	  //std::cout << "*** Name: " << textures[i].mName << std::endl;
	  //std::cout << "Flags: " << textures[i].mFlags << " Contents: " << textures[i].mContents << std::endl;
	  
	  // Quake 3 uses jpeg and tga textures but mName does not store the file
	  // ending, so we cannot tell directly which loader to call. Estimate the
	  // file ending first.
	  
	  //std::cout << "Try to load texture: " << textures[i].mName << " as JPEG" << std::endl;
	  bool isJPEG = false;
	  bool found = false;
	  std::string s(".//Data//");
	  s.append(textures[i].mName);
	  s.append(".jpg");
	  // std::ios::ate - set the initial position to the end of the file
	  std::ifstream file;
	  file.open(s.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	  if (!file.is_open()) {
		  //std::cout << textures[i].mName << " not found as JPEG, it has to be TGA" << std::endl;
	  } else {
	    //std::cout << textures[i].mName << " found as JPEG" << std::endl;
	    isJPEG = true;
	    found = true;
	    file.close();
	  }
	  if (!found) {
	    s.clear();
	    s.append(".//Data//");
	    s.append(textures[i].mName);
	    s.append(".tga");
	    file.open(s.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	    
	    if (!file.is_open()) {
		    //std::cout << textures[i].mName << " not found neither as JPEG nore TGA" << std::endl;
	    } else {
	      //std::cout << textures[i].mName << " found as TGA" << std::endl;
	      isJPEG = false;
	      found = true;
	      file.close();
	    }
	  }
	  
	  if (found) {
	    // if it is not a JPEG it has to be a tga
	    if (isJPEG) {
	      texIDtexObjMap.push_back(JPEGFile::LoadJPEGTexture(s.c_str()));
	    } else {
	      // no tga loader yet
	      texIDtexObjMap.push_back(TGAFile::LoadTGATexture(s.c_str()));
	    }
	  } else {
	    texIDtexObjMap.push_back(-1);
	  }
	  
	  //texIDtexObjMap.push_back();
	}
	
	// set pointer into the buffer onto the plane lump
	planes = (TPlane*)&(this->buffer[header->mLumps[PLANES_LUMP].mOffset]);
	if (NULL == planes) {
	  CleanUp();
		throw "cast into planes lump failed!";
	}
	int planeCount = this->header->mLumps[PLANES_LUMP].mLength / sizeof(TPlane);
	for (int i = 0; i < planeCount; i++) {
    // swap y and z and negate z
    float temp = this->planes[i].mNormal[1];
    this->planes[i].mNormal[1] = this->planes[i].mNormal[2];
    this->planes[i].mNormal[2] = -temp;
    
    // inverse distances and scale them down
    //map->planeLumps[i].Distance = -map->planeLumps[i].Distance;
    this->planes[i].mDistance /= SCALE_FACTOR;
  }
	
	// set pointer into the buffer onto the nodes lump
	nodes = (TNode*)&(this->buffer[header->mLumps[NODES].mOffset]);
	if (NULL == nodes) {
	  CleanUp();
		throw "cast into nodes lump failed!";
	}
	
	// set pointer into the buffer onto the leaves lump
	leaves = (TLeaf*)&(this->buffer[header->mLumps[LEAVES].mOffset]);
	if (NULL == leaves) {
	  CleanUp();
		throw "cast into leaves lump failed!";
	}
	leafCount = this->header->mLumps[LEAVES].mLength / sizeof(TLeaf);
	TLeaf * p = leaves;
	std::list<int> clusters;
	std::list<int>::iterator it;
	bool contained = false;
	for (int i = 0; i < leafCount; i++) {
	
	  for (it = clusters.begin(); it != clusters.end(); it++) {
	    if ((*it) == p->mCluster) {
	      contained = true;
	    }
	  }
	  
	  if (!contained) {
	    clusters.push_back(p->mCluster);
	  }
	  
	  p++;
	  contained = false;
	}
	clusterCount = clusters.size();
	//std::cout << "Cluster Count: " << clusterCount << std::endl;
	
	// Now we need to go through and convert all the leaf bounding boxes
	// to the normal OpenGL Y up axis.
	for(int i = 0; i < leafCount; i++)
	{
		// Swap the min y and z values, then negate the new Z
		int temp = leaves[i].mMins[1];
		leaves[i].mMins[1] = leaves[i].mMins[2];
		leaves[i].mMins[2] = -temp;
		
		// downscale, is that correct?
		leaves[i].mMins[0] /= SCALE_FACTOR;
		leaves[i].mMins[1] /= SCALE_FACTOR;
		leaves[i].mMins[2] /= SCALE_FACTOR;

		// Swap the max y and z values, then negate the new Z
		temp = leaves[i].mMaxs[1];
		leaves[i].mMaxs[1] = leaves[i].mMaxs[2];
		leaves[i].mMaxs[2] = -temp;
		
		// downscale, is that correct?
		leaves[i].mMaxs[0] /= SCALE_FACTOR;
		leaves[i].mMaxs[1] /= SCALE_FACTOR;
		leaves[i].mMaxs[2] /= SCALE_FACTOR;
	}
	
	// set pointer into the buffer onto the leaveface lump
	leaffaces = (TLeafFace*)&(this->buffer[header->mLumps[LEAFFACES].mOffset]);
	if (NULL == leaffaces) {
	  CleanUp();
		throw "cast into leaffaces lump failed!";
	}
	
	// set pointer into the buffer onto the leavebrush lump
	leafbrushes = (TLeafBrush*)&(this->buffer[header->mLumps[LEAFBRUSHES].mOffset]);
	if (NULL == leafbrushes) {
	  CleanUp();
		throw "cast into leafbrushes lump failed!";
	}
	
	// set pointer into the buffer onto the models lump
	models = (TModel*)&(this->buffer[header->mLumps[MODELS].mOffset]);
	if (NULL == models) {
	  CleanUp();
		throw "cast into models lump failed!";
	}
	
	// set pointer into the buffer onto the BRUSHES lump
	brushes = (TBrush*)&(this->buffer[header->mLumps[BRUSHES].mOffset]);
	if (NULL == brushes) {
	  CleanUp();
		throw "cast into brushes lump failed!";
	}
  //int brushCount = this->header->mLumps[BRUSHES].mLength / sizeof(TBrush);
	// print the number of brushsides
	//for (int i = 0; i < brushCount; i++) {
	//  std::cout << brushes[i].mNbBrushSides << std::endl;
	//}
	
	// set pointer into the buffer onto the BRUSHESSIDES lump
	brushsides = (TBrushSide*)&(this->buffer[header->mLumps[BRUSHSIDES].mOffset]);
	if (NULL == brushsides) {
	  CleanUp();
		throw "cast into brushsides lump failed!";
	}
	
	// set pointer into the buffer onto the MeshVerts lump
	meshverts = (TMeshVert*)&(this->buffer[header->mLumps[MESHVERTS].mOffset]);
	if (NULL == meshverts) {
	  CleanUp();
		throw "cast into meshverts lump failed!";
	}
	
	// set pointer into the buffer onto the effect lump
	effects = (TEffect*)&(this->buffer[header->mLumps[EFFECTS].mOffset]);
	if (NULL == effects) {
	  CleanUp();
		throw "cast into effects lump failed!";
	}
	
	// set pointer into the buffer onto the lightmaps lump
	lightmaps = (TLightMap*)&(buffer[header->mLumps[LIGHTMAPS].mOffset]);
	if (NULL == lightmaps) {
	  CleanUp();
		throw "cast into lightmaps lump failed!";
	}
	if (0 != header->mLumps[LIGHTMAPS].mLength) {
	  //std::cout << "lightmaps lump is not empty" << std::endl;
	  TLightMap * lmPointer = (TLightMap*)&(buffer[header->mLumps[LIGHTMAPS].mOffset]);
	  int lightmapCount = header->mLumps[LIGHTMAPS].mLength / sizeof(TLightMap);
	  //std::cout << "lightmap count: " << lightmapCount << std::endl;
	  lightmapArray = new unsigned int[lightmapCount];
	  for (int i = 0; i < lightmapCount; i++) {
	    // Create a texture map for each lightmap that is read in. The lightmaps
		  // are always 128 by 128.
		  CreateLightmapTexture(lightmapArray[i], 
				(unsigned char *)lmPointer->mMapData, 128, 128);
		  lmPointer++;
    }
  } else {
    std::cout << "lightmaps lump is empty" << std::endl;
    nolightmaps = true;
  }
	
	// set pointer into the buffer onto the lightvols lump
	lightvolumes = (TLightVol*)&(this->buffer[header->mLumps[LIGHTVOLS].mOffset]);
	if (NULL == lightvolumes) {
	  CleanUp();
		throw "cast into lightvolumes lump failed!";
	}
	
	
	// Some maps do not have any vis data at all!
	if (header->mLumps[VISDATA].mLength == 0) {
	  std::cout << "no vis data" << std::endl;
	  this->noVisData = true;
	} else {
	  /*if (!buffer[header->mLumps[VISDATA].mOffset]) {
	    std::cout << "no vis data" << std::endl;
	  }*/
	  // set pointer into the buffer onto the visdata lump
	  /*visdata = (TVisData*)&(this->buffer[header->mLumps[VISDATA].mOffset]);
	  if (NULL == visdata) {
	    CleanUp();
		  throw "cast into visdata lump failed!";
	  }*/
	  //// +8 is for the first two integers in the structure TVisData
	  ////visdata->mBuffer = (unsigned char*)&(buffer[header->mLumps[VISDATA].mOffset]);
	  ////visdata->mBuffer += 2*sizeof(int);
	  ////visdata->mBuffer += 8;
	  //std::cout << "Number of vectors: " << visdata->mNbClusters << std::endl;
	  //std::cout << "Bytes per vector: " << visdata->mBytesPerCluster << std::endl;
	  //for (int i = 8; i < 108; i++) {
	  //  std::cout << (int)visdata->mBuffer[i] << std::endl;
	  //}
	  //std::cout << (unsigned int)this->buffer[header->mLumps[VISDATA].mOffset] << std::endl;
  	
	  //unsigned char * pp = (unsigned char *)visdata;
  	
	  unsigned char * pp = (unsigned char *)&(this->buffer[header->mLumps[VISDATA].mOffset]);
  	
	  //std::cout << "Number of vectors: " << (int)*((int *)pp) << std::endl;
	  visdata.mNbClusters = (int)*((int *)pp);
	  pp += 4;
	  //std::cout << "Bytes per vector: " << (int)*((int *)pp) << std::endl;
	  visdata.mBytesPerCluster = (int)*((int *)pp);
	  pp += 4;
  	
	  // The mBuffer pointer is to point on the same address as the pp pointer
	  visdata.mBuffer = NULL;
	  visdata.mBuffer = pp;
  	
	  //pp += header->mLumps[VISDATA].mOffset;
	  //pp+=8;
	  /*for (int i = 0; i < 100; i++) {
	    std::cout << (int)*((int *)pp) << std::endl;
	    pp++;
	  }*/
	  
	  // Visdata
    // The visdata lump stores bit vectors that provide cluster-to-cluster 
    // visibility information. There is exactly one visdata record, with a 
    // length equal to that specified in the lump directory.
    //
    // visdata
    //
    // int n_vecs 	Number of vectors.
    // int sz_vecs 	Size of each vector, in bytes.
    // ubyte[n_vecs * sz_vecs] vecs 	Visibility data. One bit per cluster 
    // per vector.
    // 
    // Cluster x is visible from cluster y if the (1 << y % 8) 
    // bit of vecs[x * sz_vecs + y / 8] is set. 


	  /*visdata.mNbClusters = (int)buffer[header->mLumps[VISDATA].mOffset];
	  visdata.mBytesPerCluster = (int)buffer[header->mLumps[VISDATA].mOffset+4];
	  visdata.mBuffer = (unsigned char*)&(buffer[header->mLumps[VISDATA].mOffset+8]);
  	
	  std::cout << "Number of vectors: " << visdata.mNbClusters << std::endl;*/
	}
	
	//std::cout << "II [Q3BSP] loading succeeded" << std::endl;
}

void Q3BSP::ParseVector(const char * _vector, float * _dest) {
  //std::cout << "ParseVector: " << _vector << std::endl;
  //"0 0 24"
  const char * p = _vector;
  char buffer[20];
  int i = 0;
  int j = 0;
  
  while (*p != '\0') {
    
    if (*p == ' ') {
      buffer[i] = '\0';
      i = 0;
      _dest[j] = atof(buffer);
      j++;
      p++;
      continue;
    }
    
    buffer[i] = *p;
    i++;
    p++;
  }
  
  buffer[i] = '\0';
  _dest[j] = atof(buffer);
}

void Q3BSP::DrawWithoutPVS() {

  // Reset alreadyVisibleList
  ResetAlreadyVisible();
   
  // Somehow Quake3 specifies faces clockwise when frontfacing
  glFrontFace(GL_CW);
  
  for (int i = 0; i < this->leafCount; i++) {
      RenderLeaf(i);                   
  }
 
  glFrontFace(GL_CCW); 
}

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

void Q3BSP::DrawVertices() {
  glBegin(GL_POINTS);
	for (int i = 0; i < vertexCount; i++) {
	  glVertex3fv(this->vertices[i].mPosition);
	}
	glEnd();
}

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

void Q3BSP::Draw(float camera_x, float camera_y, float camera_z) {
  
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
  
  /*glVertexPointer(3, GL_FLOAT, sizeof(TVertex), vertices[0].mPosition);
  glEnableClientState(GL_VERTEX_ARRAY);
  for (int i = 0; i < faceCount; i++) {
		// Before drawing this face, make sure it's a normal polygon or a mesh
		if ((FACE_TYPE_POLYGON == faces[i].mType) || (FACE_TYPE_MESH == faces[i].mType)) {
			glDrawArrays(GL_TRIANGLE_FAN, faces[i].mVertex, faces[i].m_n_vertexes);
		}
	}*/
}

/** 
 * Iterates through all of the leaves faces. A leaf contains all faces that 
 * have non-zero intersection with the leaf volume. The faces in leaf with 
 * index L have indices leaf[L].firstFace through 
 * (leaf[L].firstFace + leaf[L].facesCount - 1). 
 */
void Q3BSP::RenderLeaf(int _leafIndex) {
  
  TLeaf l;
  int i;
  int faceIndex;
  TFace f;
  
  l = this->leaves[_leafIndex];
  
  for (i = l.mLeafFace; i < l.mLeafFace + l.mNbLeafFaces; i++) {
    
    // Get the index of the face out of the leaveFace Array
    faceIndex = this->leaffaces[i].mFaceIndex;
    
    //// DEBUG
    //if (0 == faceIndex) {
    //  std::cout << "rendering " << faceIndex << " for leaf " << _leafIndex << std::endl;
    //  RenderPolygonFaceDebug(this->faces[faceIndex]);
    //  //MakeAlreadyVisible(0);
    //  continue;
    //}
    
    if (!IsAlreadyVisible(faceIndex)) {
      
      MakeAlreadyVisible(faceIndex);
      
      f = this->faces[faceIndex];
      
      // Find out about the face type
      // face type: 1 = Polygon, 2 = Patch, 3 = Mesh, 4 = Billboard.
      switch (f.mType) {
        case FACE_TYPE_POLYGON:
          //RenderPolygonFaceDebug(f);
          RenderPolygonFace(f);
          break;
          
        case FACE_TYPE_PATCH:
          RenderPatchFaceDebug(faceIndex);          
          break;
          
        case FACE_TYPE_MESH:
          //RenderPolygonFaceDebug(f);
          RenderPolygonFace(f);
          break;
          
        case FACE_TYPE_BILLBOARD:     
          //std::cout << "[DEBUG] BILLBOARD found!" << std::endl;     
          break;
          
        default:
          //printf("[DEBUG] unknown face type detected\n");
          std::cout << "[DEBUG] unknown face type detected: " << f.mType << std::endl;
          break;
      }
    } 
  }
}

void Q3BSP::RenderMeshFace(TFace f) {
}

void Q3BSP::RenderPolygonFace(TFace _f) {
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
  
  /*glDrawArrays(GL_TRIANGLE_FAN, this->meshverts[_f.mMeshvert].mMeshVert,
               _f.m_n_meshverts);*/

  
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
}

void Q3BSP::RenderPolygonFaceDebug(TFace f) {
  
  //int stride;
  
  
  /*stride = sizeof(Vertex);
  glVertexPointer(3, GL_FLOAT, stride, &(map->vertexLumps[0].Position));
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_TRIANGLE_FAN, f.FirstVertex, f.VertexCount);*/
  
  /*determine the size of one BSP Vertex structure*/
  /*stride = sizeof(Vertex);*/ 
  /* Set the pointer on the very first Vertex in the Vertex Lump */
  /*glVertexPointer(3, GL_FLOAT, stride, &(map->vertexLumps[0].Position));*/
  /*glClientActiveTextureARB(GL_TEXTURE0_ARB);
  glTexCoordPointer(2, GL_FLOAT, stride, &(vertex[offset].textureCoord));
  glClientActiveTextureARB(GL_TEXTURE1_ARB);
  glTexCoordPointer(2, GL_FLOAT, stride, &(vertex[offset].lightmapCoord));*/
  /*glEnableClientState(GL_VERTEX_ARRAY);*/
  /*glDrawElements(GL_TRIANGLES, f.MeshVertexCount,
  GL_UNSIGNED_INT, &(map->meshVertLumps[f.FirstMeshVertex]));*/
  /* Draw using direkt indizes in the vertex array */
  /*glDrawArrays(GL_TRIANGLE_FAN, f.FirstVertex, f.VertexCount);*/
  
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
  
  /*glVertexPointer(3, GL_FLOAT, sizeof(TVertex), this->vertices);
  glNormalPointer(GL_FLOAT, sizeof(TVertex), this->vertices->mNormal);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  for (int i = 0; i < this->faceCount; i++) {
		// Before drawing this face, make sure it's a normal polygon or a mesh
//		if ((1 == map->faceLumps[i].Type) || (3 == map->faceLumps[i].Type)) {
			glDrawArrays(GL_TRIANGLE_FAN, this->faces[i].mVertex, this->faces[i].m_n_vertexes);
		//}
	}*/
}

/**
 * In a patch face (type == 2), there is a index into the vertex data
 * (= int mVertex) // Index of first vertex.
 * and a number of vertices (int m_n_vertexes;) stored. The vertices 
 * starting at that first vertex describe a Grid of control points.
 *
 * The Grid is stored as a twodimensional matrix of vertices.
 * The Grid�s dimensions cannot be deduced just by the number of 
 * vertices it contains. As the dimensions matter, they are stored
 * seperately in int mSize[2];
 *
 * The Grid is subdivided into 3x3 patches. A patch is describing a
 * biquadratic bezier surface by specifing three quadratic bezier
 * curves.
 *
 * To compute vertices on the surface, you have to tesselate it.
 * (Otherwise you could only draw using the vertices stored in
 * the BSP file, which gives you a very poor visual for your map!)
 * Tesselation is a two tiered interpolation of control points.
 * First, control points on the three quadratic bezier
 * curves are interpolated and form the control points of a new
 * quadratic bezier curve, which by second tier interpolation gives you
 * a point inside the surfce.
 *
 * This method iterates over all 3x3 patches in the Grid and stores their
 * control points into Bezier object. The Bezier class handles
 * the two tiered interpolation (= tesselation) and later is used for
 * rendering the biquadratic bezier surface.
 */
void Q3BSP::TesselatePatches(int _tesselationEdges) {
  TFace f;
  Bezier * b;
  std::vector<Bezier *> * bezierVector;

  //std::cout << "TesselatePatches() ..." << std::endl;

  for (int faceIndex = 0; faceIndex < faceCount; faceIndex++) {
    f = faces[faceIndex];
		switch (f.mType) {
      case FACE_TYPE_PATCH:
          //// Print info
          //std::cout << "****** Face " << faceIndex << " is patch" << std::endl;
          //std::cout << "Vertices involved: " << f.m_n_vertexes << std::endl;
          //std::cout << "Grid dimensions: size[0]: " << f.mSize[0]
          //  << " size[1]: " << f.mSize[1] << std::endl;
          //std::cout << "Number of patches: " << ((f.mSize[0]-1)/2) 
          //  << " " << ((f.mSize[1]-1)/2) << std::endl;

          // Create Bezier Vector
          bezierVector = new std::vector<Bezier *>();

          // loop over all patches of a Grid�s control points
          for (int height = 0; height < ((f.mSize[1]-1)/2); height++) {
            for (int width = 0; width < ((f.mSize[0]-1)/2); width++) {

              // Create Bezier object
              b = new Bezier();

              // This nested inner loop reads a 3x3 subarea out of the
              // Grid. width and height tell you, where the current subarea
              // begins. The formula (2*height * f.mSize[0] + 2*width) computes,
              // the the index into the vertices array for this subarea.
              // Note that the height has to be multiplied by the grid width mSize[0],
              // as it takes to jump over mSize[0] elements to switch a row.
              // 2*width is basically an offset local to a row.
              //
              // The row*f.mSize[0] + point part, describes the 3x3 subarea form
              // It also skips entire rows (row*f.mSize[0]) and uses offsets local 
              // to rows (point).
              //
              // The whole Grid is best imagined, by imagining a matrix of width
              // mSize[0] and height mSize[1]. The rows of that matrix are stored
              // in consective order begining at mVertex and using the next
              // m_n_vertexes vertices.
              for(int row = 0; row < 3; row++)
				      {
					      for(int point = 0; point < 3; point++)
					      {
                  b->controls[row*3 + point] = vertices[f.mVertex +
                    (2*height * f.mSize[0] + 2*width) +
									  row*f.mSize[0] + point];
                }
              }

              // Tesselate
              b->tessellate(_tesselationEdges);

              // insert Bezier into Bezier vector
              bezierVector->push_back(b);
            }
          }

          // Map faceIndex to the Bezier objects
          bezierMap.insert(std::make_pair(faceIndex, bezierVector));
				break;

			default:
				break;
		}
	}

  //std::cout << "TesselatePatches() finished" << std::endl;
}

/**
 * Renders the Bezier objects, which correspond to the
 * face in the face array at position _faceIndex.
 *
 * The _faceIndex is normally used to index into the faces array
 * faces[_faceIndex]. The parsing method prepares a mapping from
 * _faceIndex to Bezier objects. If a _faceIndex is mapped to
 * a bezier, the face desribed by _faceIndex is said to correspond
 * to the Bezier _faceIndex is mapped to.
 *
 * param _faceIndex - index into Q3BSP::faces array. 
 */
void Q3BSP::RenderPatchFaceDebug(int _faceIndex) {
  std::vector<Bezier *>::iterator it;

  /*if (activeFaceIndex != _faceIndex) {
    glColor3f(1.0f, 1.0f, 1.0f);
  } else {
    glColor3f(1.0f, 0.0f, 0.0f);
  }*/

  if (NULL == bezierMap[_faceIndex]) {
    return;
  }

  // use _faceIndex to map into the Bezier mapping
  //std::cout << "ERROR: RenderPatchFaceDebug() uncomment me!" << std::endl;
  for (it = bezierMap[_faceIndex]->begin(); 
    it != bezierMap[_faceIndex]->end();
    it++)
  {
    (*it)->render();
    //(*it)->renderDebug();
  }
}

void Q3BSP::ResetAlreadyVisible() {  
  for (int i = 0; i < this->bytesInAlreadyVisibleList; i++) {
    this->alreadyVisible[i] = 0;
  }
}

/**
 * If the cluster with index A can potentially be seen by a viewer in the 
 * cluster with index B, then bit (a + b * visData.sz_vecs * 8) of 
 * visData.vecs has value 1. Otherwise, that bit has value 0.
 *
 * this->visdata.mNbClusters = number of vectors (every cluster has its vector)
 * this->visdata.mBytesPerCluster = bytes per vector (on bit per other vector)
 * this->visdata.mBuffer = begining of the visdata vectors
 */
//bool Q3BSP::IsPotentiallyVisible(int _camCluster, int _testCluster) {
//  int i;
//  unsigned char visSet;
//  
//  if (_camCluster < 0) {
//    return true;
//  }
//  
//  //i = (camCluster * this->visdata->mBytesPerCluster) + (testCluster >> 3);
//  //visSet = this->visdata->mBuffer[i];
//  
//  i = (_camCluster * this->visdata.mBytesPerCluster) + (_testCluster >> 3);
//  visSet = this->visdata.mBuffer[i];
//
//  return (visSet & (1 << (_testCluster & 7))) != 0;
//  //return (visSet & (128 >> (_testCluster & 7))) != 0;
//}

bool Q3BSP::IsPotentiallyVisible(int _camCluster, int _testCluster) {

  //std::cout << _camCluster << " " << _testCluster << std::endl;
  
  if ((_camCluster < 0) || noVisData) {
    return true;
  }
  
  unsigned char visSet = visdata.mBuffer[(_camCluster * (visdata.mBytesPerCluster)) + (_testCluster / 8)];
  int result = visSet & (1 << ((_testCluster) & 7));
  
  /*if (result == 1 && (_testCluster != -1)) {
    std::cout << _camCluster << " not see " << _testCluster << std::endl;
  }*/
  
  return (result != 0);
}

/**
 * returns non 0 if the face has been rendered before.
 * returns 0 if the face has to be rendered.
 */
int Q3BSP::IsAlreadyVisible(int _faceIndex) {

  //return 0;
  
  int fullpart;
  
  if (_faceIndex == 0) {
    /* Face 0 is encoded as the very first bit in the alreadyVisible list
       128 = 10000000 */
    return this->alreadyVisible[0] & 128; 
  } else {
    fullpart = _faceIndex / 8;
    //return (this->alreadyVisible[fullpart] & (1 << (_faceIndex & 7)));
    return (this->alreadyVisible[fullpart] & (128 >> (_faceIndex & 7)));
  }
  return 0;
}

/**
 * Call this function with the faceIndex of the face that has currently
 * been rendered, so that you can later ask the AlreadyVisible list,
 * if a face has been already rendered or if that face has to be rendered.
 * Use the isAlreadyVisible() list to find out, wether a face has to be
 * rendered or not.
 */   
void Q3BSP::MakeAlreadyVisible(int _faceIndex) {
  
  //std::cout << (128 >> 1) << std::endl;
  int fullpart;
  
  if (0 == _faceIndex) {
    this->alreadyVisible[0] |= (1 << 7);
  } else {
    fullpart = (_faceIndex) / 8;
    //if (fullpart > map->BytesInAlreadyVisibleList) {
    //  printf("[DEBUG] invalid fullpart. faceCount: %d, faceIndex: %d\n", map->faceCount, faceIndex);
    //  return;
    //}
    //this->alreadyVisible[fullpart] |= (1 << (_faceIndex & 7));
    this->alreadyVisible[fullpart] |= (128 >> (_faceIndex & 7));
  }
}

/**
 * Returns the Index of the leave that the camera is currently 
 * positioned inside
 */
int Q3BSP::GetCurrentLeafIndex(float camPos[3]) {
  
  int index;
  float distance;
  TNode node;
  TPlane plane;  

  index = 0;
  
  while (index >= 0) {
    node = this->nodes[index];
    plane = this->planes[node.mPlane];

    // Distance from point to a plane
    distance = DotProduct(plane.mNormal, camPos) - plane.mDistance;

    // if distance is positive, then the camPos is in front of the plane.
    if (distance >= 0) {
      // child 0 contains the plains front region
      index = node.mChildren[0];
    } else {
      // child 1 contains the plains back region
      index = node.mChildren[1];
    }
  }
  
  return (-1)*index - 1;
}

void Q3BSP::CleanUp() {

  //std::cout << "CleanUp() Q3BSP data structures ..." << std::endl;

  std::map<int, std::vector<Bezier *> *>::iterator iter;
  std::map<int, std::vector<Bezier *> *>::iterator tempIter;
  std::vector<Bezier *>::iterator bVIter;
  std::vector<Bezier *>::iterator tempBVIterator;
  bool done = false;
  bool bVDone = false;

  //std::cout << "Start deleting beziermap ..." << std::endl;
  if (!bezierMap.empty()) {
    
    iter = bezierMap.begin();

	  while (!done) {	
	    tempIter = iter;
	    // if it == partList.end() it doesn't point on any object.
	    // Then it is invalid and points "behind" the last object of the list
	    if (iter == bezierMap.end()) {
	      done = true;
	      continue;
	    }
	    iter++;
	    
      // clear the content of the bezier vector, i.e. delete all
      // Bezier objects stored in it
      std::vector<Bezier *> * bezierVector = (*tempIter).second;
      if (NULL != bezierVector) {
        bVIter = bezierVector->begin();
        while (!bVDone) {
          tempBVIterator = bVIter;
          if (bVIter == bezierVector->end()) {
	          bVDone = true;
	          continue;
	        }
	        bVIter++;          
          if (NULL != *tempBVIterator) {
            delete (Bezier *)((*tempBVIterator));
          }
        }
        // free memory for the vector itself
	      delete (std::vector<Bezier *> *)((*tempIter).second);
      }

      // erase() needs the iterator, not the element 
      // the iterator points to!
		  bezierMap.erase(tempIter);
	  }  
  }
  //std::cout << "beziermap deleted" << std::endl;

  //std::cout << "Start deleting buffer ..." << std::endl;
	if (NULL != this->buffer) {
		delete[] this->buffer;
		this->buffer = NULL;
	}
  //std::cout << "buffer deleted" << std::endl;

  //std::cout << "Start deleting alreadyVisible ..." << std::endl;
	if (NULL != this->alreadyVisible) {
	  delete[] this->alreadyVisible;
		this->alreadyVisible = NULL;
	}
  //std::cout << "beziermap alreadyVisible" << std::endl;

	if (NULL != this->lightmapArray) {
	  delete [] lightmapArray;
	  lightmapArray = NULL;
	}
	this->entities = NULL;
	this->vertices = NULL;
	this->faces = NULL;
	this->header = NULL;
	this->textures = NULL;
	this->planes = NULL;
	this->nodes = NULL;
	this->leaves = NULL;
	this->leaffaces = NULL;
	this->leafbrushes = NULL;
	this->models = NULL;
	this->brushes = NULL;
	this->brushsides = NULL;
	this->meshverts = NULL;
	this->effects = NULL;
	this->lightmaps = NULL;
	this->lightvolumes = NULL;
	this->visdata.mBuffer = NULL;
	//this->visdata = NULL;
	
	this->vertexCount = 0;
	this->faceCount = 0;
	this->leafCount = 0;
	currentFace = 0;
	currentCluster = 0;
	noVisData = false;
	nolightmaps = false;
	this->info_player_start[0] = 0.0f;
	this->info_player_start[1] = 0.0f;
	this->info_player_start[2] = 0.0f;

  //std::cout << "Q3BSP data structures cleaned" << std::endl;
	gamma = 10.0f;
}

/**
 * param _brushIndex - index into the brushes lump which stores TBrush
 *   structures
 */
//void Q3BSP::DrawBrush(int _brushIndex) {
//  TBrush * brush = NULL;
//  
//  brush = &this->brushes[_brushIndex];
//  
//  // some brushes do not have any sides!
//  if (0 >= brush->mNbBrushSides) {
//    std::cout << "Brush has no brushsides associated!" << std::endl;
//    return;
//  }
//}

/**
 * Sets vector outputEnd, which is the point of first collision along the 
 * trace line. Sets outputFraction, which is the fraction of trace line
 * until first collision. Sets outputStartsOut and outputAllSolid
 */
void Q3BSP::Trace(float * _inputStart, float * _inputEnd) {

  outputFraction = 1.0f;
  
  /*std::cout << "Trace(): " << 
    _inputStart[0] << " | " << 
    _inputStart[1] << " | " <<
    _inputStart[2] << 
    std::endl;*/
    
  this->inputStart = _inputStart;
  this->inputEnd = _inputEnd;
  
  // walk through the BSP tree
  // void CheckNode(int _nodeIndex, 
	//                float _startFraction, 
	//                float _endFraction, 
	//                float * _start, 
  //                float * _end);
  CheckNode(0, 0.0f, 1.0f, _inputStart, _inputEnd);

  if (outputFraction == 1.0f) {
    // nothing blocked the trace
    this->outputEnd[0] = _inputEnd[0];
    this->outputEnd[1] = _inputEnd[1];
    this->outputEnd[2] = _inputEnd[2];
  } else { 
    // collided with something
    this->outputEnd[0] = _inputStart[0] + outputFraction * (_inputEnd[0] - _inputStart[0]);
    this->outputEnd[1] = _inputStart[1] + outputFraction * (_inputEnd[1] - _inputStart[1]);
    this->outputEnd[2] = _inputStart[2] + outputFraction * (_inputEnd[2] - _inputStart[2]);
  }
}

/**
 * Fractions have to be fractions of the total trace line! In every recursion,
 * you have to compute fractions of the total trace line, do not insert fractions
 * relative to the node which is checked. _start and _end have to be points
 * of the total trace line which correspon to the total fractions of the trace
 * line
 */
void Q3BSP::CheckNode(int _nodeIndex, 
  float _startFraction, float _endFraction, 
  float * _start, float * _end) 
{
  TLeaf leaf;
  TBrush brush;
  TNode node;
  TPlane plane;
  float startDistance;
  float endDistance;
  float inverseDistance;

  // End of recursion condition: did the algorithm arrive at a leave ?
  if (_nodeIndex < 0) {   

    // this is a leaf
    leaf = leaves[-(_nodeIndex + 1)];

    for (int i = 0; i < leaf.mNbLeafBrushes; i++) {

      brush = brushes[leafbrushes[leaf.mLeafBrush + i].mBrushIndex];

      if ((brush.mNbBrushSides > 0) && 
        (textures[brush.mTextureIndex].mContents & CONTENTS_SOLID)) 
      {
        //std::cout << "CheckNode: calling CheckBrush()" << std::endl;
        CheckBrush(brush);
      }
    }

    // don't have to do anything else for leaves
    return;
  }

  // The current node is no leave so keep recursive checking
  node = nodes[_nodeIndex];
  plane = planes[node.mPlane];

  // Hier wird die Koordinatengleichung einer Ebene angewendet. Wenn plane.Normal
  // normalisiert ist (und nur dann !) gibt plane.Distance in der Koordinaten-
  // gleichung den Abstand der Ebene zum Nullpunkt an! Der Abstand einer Ebene
  // zu einem Punkt ist definiert als der Abstand des n�chsten Punktes auf der 
  // Ebene zu einem Punkt, als dem Punkt, den die Normale der Ebene auf der
  // Ebene treffen w�rde, wenn man die Normale auf dem Urprung aufstellen w�rde.
  // Falls dies der Fall ist, (Normale normalisiert)
  // und man einen Punkt, hier start, in die Gleichung einsetzt, so erh�lt man
  // eine Zahl, die Aufschluss dar�ber gibt, ob der Punkt auf der Seite der Ebene
  // liegt, in die die Normale zeigt, wenn man sie auf die Ebene stellen w�rde
  // (Ergebnis: negativ) oder auf der Seite, in 
  // die die Normale nicht zeigt (Ergebnis: positiv) 
  // oder genau auf der Ebene (Ergebnis == 0)
  startDistance = DotProduct(_start, plane.mNormal) - plane.mDistance;
  endDistance = DotProduct(_end, plane.mNormal) - plane.mDistance;

  if (startDistance >= 0.0f && endDistance >= 0.0f) {
    // Case A: both points are in front of the plane so check the front child only
    // front means on the side in which the normal is facing, when it is put on
    // some point of the plane
    CheckNode(node.mChildren[0], _startFraction, _endFraction, _start, _end);
  }
  else if (startDistance < 0.0f && endDistance < 0.0f) {
    // Case B: both points are behind the plane so check the back child only
    // back means on the side in which the normal is not facing, when it is put on
    // some point of the plane
    CheckNode(node.mChildren[1], _startFraction, _endFraction, _start, _end);
  }
  else { 
    // Case C: the line spans the splitting plane
    //   At this point because of the two above if statments, we know that
    //   one of the distances is positive and the other distance is negative.
    //   It is posible that our trace line starts behind the plane and travels
    //   towards the end point which is in front of the plane. Or it is possible
    //   that the start point lies in front of the plane and travels through the
    //   plane towards the end point which lies in the back of the plane. These 
    //   two situations are checked right now. There is a third situation where
    //   both distances are the same value. This means ??? this is imposible 
    //   isn't it ?
    
    int side;
    float fraction1;
    float fraction2;
    float middleFraction;
    float middle[3];

    // STEP 1: split the segment into two 
    if (startDistance < endDistance) {

      // side tells us, which side the startPoint lies on, 0 is front, 1 is back
      // this information is needed for?
      side = 1; // back

      // As startDistance is negativ as in this case start lies behind the
      //   plane and endDistance is positive the denominator adds up to the
      //   complete distance between the start and the end point but with 
      //   negative sign. 1.0f is divided by the complete negative distance
      //   in order to compute the fraction in the next line.
      inverseDistance = 1.0f / (startDistance - endDistance);

      //// startDistance is negativ and the addition of EPSILON doesn't make it
      //   positive as EPSILON is so small. If we multiply inverseDistance to
      //   it, which is negative itself, we get a positive value again. 
      //
      //   fraction1
      //   tells us how far we have to travel to find the intersection point of
      //   the trace line with the plane. 
      // GUESS: fraction1 is on the negative distance side EPSILON away from the plane
      // GUESS: fraction2 is on the positive distance side EPSILON away from the plane
      fraction1 = (startDistance + Q3EPSILON) * inverseDistance;

      // why is fraction2 needed? it is the exact same value as fraction1
      fraction2 = (startDistance + Q3EPSILON) * inverseDistance;
      //fraction2 = (startDistance - Q3EPSILON) * inverseDistance;

    } else if (endDistance < startDistance) {

      side = 0; // front

      // endDistance is negative so the line gives a positive value.
      //   The denominator tells us how long the distance between the two
      //   points is.
      inverseDistance = 1.0f / (startDistance - endDistance);

      // fraction1 tells us how long to travel to find the intersection point 
      // GUESS: fraction1 is on the negative distance side Q3EPSILON away from the plane 
      // GUESS: fraction2 is on the positive distance side Q3EPSILON away from the plane 
      fraction1 = (startDistance + Q3EPSILON) * inverseDistance;
      fraction2 = (startDistance - Q3EPSILON) * inverseDistance;

    } 
    //else {
    //  printf("[DEBUG] collision detection this is impossible !\n");
    //  side = 0;
    //  fraction1 = 1.0f;
    //  fraction2 = 0.0f;
    //}

    // STEP 2: make sure the numbers are valid
    if (fraction1 < 0.0f) {
      fraction1 = 0.0f;
    } else if (fraction1 > 1.0f) {
      fraction1 = 1.0f;
    }

    if (fraction2 < 0.0f) {
      fraction2 = 0.0f;
    } else if (fraction2 > 1.0f) {
      fraction2 = 1.0f;
    }

    // STEP 3: calculate the middle point for the first side. As we have to put
    //   global fractions and global points into the checkNode() function, but the
    //   fraction1 and fraction2 values are lokal between the two points that 
    //   are in front an behind the plain, we have to convert the lokal fraction
    //   into a global fraction that is applicable to the complete trace line 
    //   from its start to its end point. The following line does the conversion
    //   to global fractions. 
    middleFraction = _startFraction + (_endFraction - _startFraction) * fraction1;

    // Here the global middle point on the trace line is computed. The middle
    //   point is the point, where the trace line intersects with the current
    //   nodes splitting plane. This global point can be computed from the
    //   lokal fraction, as it makes no difference to find the same point from
    //   lokal or global fractions. 
    for (int i = 0; i < 3; i++) {
      middle[i] = _start[i] + fraction1 * (_end[i] - _start[i]);
    }

    // STEP 4: check the first side - the inserted fractions always have to
    //   be percentages of the total trace line!
    CheckNode(node.mChildren[side], 
      _startFraction, middleFraction, 
      _start, middle);

    // STEP 5: calculate the middle point for the second side 
    middleFraction = _startFraction + (_endFraction - _startFraction) * fraction2;
    for (int i = 0; i < 3; i++) {
      middle[i] = _start[i] + fraction2 * (_end[i] - _start[i]);
    }

    // STEP 6: check the second side - the inserted fractions always have to
    //   be percentages of the total trace line!
    CheckNode(node.mChildren[!side], 
      middleFraction, _endFraction, 
      middle, _end);  
  }
}

/**
 * param _inputStart - start point of the total trace line
 * param _inputEnd - end point of the total trace line
 */
//void Q3BSP::CheckBrush(TBrush _brush, float * _inputStart, float * _inputEnd) {
void Q3BSP::CheckBrush(TBrush _brush) {

  TBrushSide brushSide;
  TPlane plane;
  float startFraction = -1.0f;
  float endFraction = 1.0f;
  bool startsOut = false;
  bool endsOut = false;
  float startDistance = 0.0f;
  float endDistance = 0.0f;
  float fraction = 0.0f;
  
  /*std::cout << "CheckBrush(): " << 
    this->inputStart[0] << " | " << 
    this->inputStart[1] << " | " <<
    this->inputStart[2] << 
    std::endl;*/
  
  
  for (int i = 0; i < _brush.mNbBrushSides ; i++) {

    // Every brushside is associated with exactly one plane
    brushSide = brushsides[_brush.mBrushSide + i];
    plane = planes[brushSide.mPlaneIndex];

    //startDistance = DotProduct(_inputStart, plane.mNormal) - plane.mDistance;
    //endDistance = DotProduct(_inputEnd, plane.mNormal) - plane.mDistance;
    
    startDistance = DotProduct(this->inputStart, plane.mNormal) - plane.mDistance;
    endDistance = DotProduct(this->inputEnd, plane.mNormal) - plane.mDistance;

    if (startDistance > 0.0f) {
      startsOut = true;
    }
    if (endDistance > 0.0f) {
      endsOut = true;
    }
    
    /*if ((startsOut) && (!endsOut)) {
      std::cout << "found plane that is being crossed" << std::endl;
    }*/

    // make sure the trace isn't completely on one side of the brush 
    if ((startDistance > 0.0f) && (endDistance > 0.0f)) {
      // both are in front of the plane, its outside of this brush 
      return;
    }

    if ((startDistance <= 0.0f) && (endDistance <= 0.0f)) { 
      // both are behind this plane, it will get clipped by another one 
      continue;
    }

    // At this point we know that one point is in front and the other 
    //   point is behind the plane, because of the above tests haven't 
    //   returned from the function or haven't skipped that loop's repetition 
    if (startDistance > endDistance) {

      // line is entering into the brush. We know that either startDistance or
      //   end distance is positive. The other one is negative. So in this case
      //   startDistance > endDistance the starting point must lie in front
      //   of the plane and end distance behind the plane. So the line enters
      //   the brush.
      fraction = (startDistance - Q3EPSILON) / (startDistance - endDistance);
      
      //std::cout << (float)fraction << std::endl;

      if (fraction > startFraction) {
        startFraction = fraction;
      }
      
    } else { 

      // line is leaving the brush
      fraction = (startDistance + Q3EPSILON) / (startDistance - endDistance);

      if (fraction < endFraction) {
        endFraction = fraction;
      }
    }
  }

  if (!startsOut) {
    this->outputStartsOut = false;
    if (!endsOut) {
      this->outputAllSolid = true;
    }
    return;
  }

  if (startFraction < endFraction) {
    if ((startFraction > -1.0f) && (startFraction < this->outputFraction)) {
    
      if (startFraction < 0.0f) {
        startFraction = 0.0f;
      }
      
      this->outputFraction = startFraction;
    }
  }
}

float Q3BSP::DotProduct(float * v1, float * v2) {
  return (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}

void Q3BSP::DumpLeavesContainingFace(int _faceIndex) {
  for (int i = 0; i < leafCount; i++) {
  
    TLeaf leaf = leaves[i];
    
    for (int j = 0; j < leaf.mNbLeafFaces; j++) {
    
      int faceIndex = this->leaffaces[leaf.mLeafFace + j].mFaceIndex;
      
      if (_faceIndex == faceIndex) {
        std::cout << "()()() Leaf " << i << " contains face " << _faceIndex << std::endl;
      }
    }
  }
}

void Q3BSP::RenderCluster(int _clusterIndex) {
  for (int i = 0; i < leafCount; i++) {
    if (leaves[i].mCluster == _clusterIndex) {
      RenderLeaf(i);
    }
  }
}

/**
 * This manually changes the gamma of an image
 */
void Q3BSP::ChangeGamma(unsigned char * _pImage, int _size, float _factor) {
	// Go through every pixel in the lightmap
	for(int i = 0; i < _size / 3; i++, _pImage += 3) 
	{
		float scale = 1.0f;
		float temp = 0.0f;
		float r = 0;
		float g = 0;
		float b = 0;

		// extract the current RGB values
		r = (float)_pImage[0];
		g = (float)_pImage[1];
		b = (float)_pImage[2];

		// Multiply the factor by the RGB values, while keeping it to a 255 ratio
		r = r * _factor / 255.0f;
		g = g * _factor / 255.0f;
		b = b * _factor / 255.0f;
		
		// Check if the the values went past the highest value
		if ((r > 1.0f) && ((temp = (1.0f/r)) < scale)) {
		  scale = temp;
		}
		if (g > 1.0f && (temp = (1.0f/g)) < scale) scale = temp;
		if (b > 1.0f && (temp = (1.0f/b)) < scale) scale = temp;

		// Get the scale for this pixel and multiply it by our pixel values
		scale *= 255.0f;		
		r *= scale;	g *= scale;	b *= scale;

		// Assign the new gamma'nized RGB values to our image
		_pImage[0] = (unsigned char)r;
		_pImage[1] = (unsigned char)g;
		_pImage[2] = (unsigned char)b;
	}
}

/**
 * This creates a texture map from the light map image bits
 */
void Q3BSP::CreateLightmapTexture(unsigned int & _textureObjectID, unsigned char * _pImageBits, 
  int _width, int _height)
{
	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &_textureObjectID);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, _textureObjectID);

	// Change the lightmap gamma values by our desired gamma
	ChangeGamma(_pImageBits, _width * _height * 3, gamma);

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, _pImageBits);

	//Assign the mip map levels		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
