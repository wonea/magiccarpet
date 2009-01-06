#include "MS3DLoader.h"

wbLib::ms3d::MS3DLoader::MS3DLoader() {
  buffer = NULL;
  meshes = NULL;
  joints = NULL;
  cleanUp();
}

/**
 * Loads a md2 and its texture
 */
wbLib::ms3d::MS3DLoader::MS3DLoader(const std::string _fileName) {
  buffer = NULL;
  meshes = NULL;
  joints = NULL;
	cleanUp();

  performFileHandling(_fileName);
	parse();
}

/**
 * Loads a md2 and its texture
 */
void wbLib::ms3d::MS3DLoader::load(const std::string _fileName) {
  buffer = NULL;
  meshes = NULL;
  joints = NULL;
	cleanUp();

  performFileHandling(_fileName);
	parse();
}

/**
 * Opens the file, loads content into buffer
 *
 * returns -1 in case of error, returns 0 in case of success
 */
void wbLib::ms3d::MS3DLoader::performFileHandling(const std::string _fileName) {
	buffer = NULL;

	// std::ios::ate - set the initial position to the end of the file
	std::ifstream file;
	file.open(_fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
    std::ios_base::failure f("MS3DLoader::performFileHandling() - Could not find model file");
    throw f;
	}
	int size = file.tellg();
	file.seekg (0, std::ios::beg);

	buffer = new char[size];
	file.read(buffer, size);

	if (!file.good()) {
		delete[] buffer;
		buffer = NULL;
    file.close();
    
    std::ios_base::failure f("MS3DLoader::performFileHandling() - Could not read from model file");
    throw f;
	}

	file.close();
}

void wbLib::ms3d::MS3DLoader::parse() throw (std::ios_base::failure) {
  const char * ptr = NULL;
  wbLib::ms3d::MS3D_Header * header = NULL;

  if (NULL == buffer) {
    std::ios_base::failure f("MS3DLoader::parse() - buffer not filled");
    throw f;
  }

  // set pointer on buffer so that original buffer pointer 
  // stays untouched for memory deallocation
  ptr = buffer;

  // cast to MS3D_Header
	header = (wbLib::ms3d::MS3D_Header *)ptr;
  
  // advance pointer
	ptr += sizeof(wbLib::ms3d::MS3D_Header);

  if (strncmp(header->id, "MS3D000000", 10) != 0) {
    cleanUp();
    std::cout << "MS3DLoader::parse() - Not A Valid Milkshape3D Model File." << std::endl;
    std::ios_base::failure f("MS3DLoader::parse() - Not A Valid Milkshape3D Model File.");
    throw f;
  } else {
    std::cout << "id: " << header->id << std::endl;
  }

  if (header->version < 3 || header->version > 4) {
    cleanUp();
    std::cout << "MS3DLoader::parse() - Unhandled File Version. Only Milkshape3D Version 1.3 And 1.4 Is Supported." << std::endl;
    std::ios_base::failure f("MS3DLoader::parse() - Unhandled File Version. Only Milkshape3D Version 1.3 And 1.4 Is Supported.");
    throw f;
  } else {
    std::cout << "version: " << header->version << std::endl;
  }

  // read number of vertices 2 byte
  numVertices = *(unsigned short *)ptr;
  // advance pointer
  ptr += sizeof(unsigned short);
  std::cout << "Number of Vertices: " << numVertices << std::endl;

  // set pointer onto vertices
  vertices = (MS3D_Vertex *)ptr;
  // advance pointer
  ptr += (sizeof(MS3D_Vertex) * numVertices);

  // read number of triangles 2 byte
  numTriangles = *(unsigned short *)ptr;
  // advance pointer
  ptr += sizeof(unsigned short);
  std::cout << "Number of Triangles: " << numTriangles << std::endl;

  // set pointer onto triangles
  triangles = (MS3D_Triangle *)ptr;
  // advance pointer
  ptr += (sizeof(MS3D_Triangle) * numTriangles);

  // swizzle t texture coordinate from ms3d spec to OpenGL convention
  for (int i = 0; i < numTriangles; i++) {
    triangles[i].t[0] = 1.0f - triangles[i].t[0];
    triangles[i].t[1] = 1.0f - triangles[i].t[1];
    triangles[i].t[2] = 1.0f - triangles[i].t[2];
  }

  // read number of groups (aka meshes) (2 byte)
  numMeshes = *(unsigned short *)ptr;
  // advance pointer
  ptr += sizeof(unsigned short);
  std::cout << "Number of Meshes: " << numMeshes << std::endl;

  // read groups/meshes
  meshes = new MS3D_Group[numMeshes];
  for (int i = 0; i < numMeshes; i++) {
    meshes[i].flags = *(unsigned char *)ptr;
    ptr += sizeof(unsigned char);

    meshes[i].name = (char *)ptr;
    ptr += (sizeof(unsigned char) * 32);
    //ptr += 32;
    //std::cout << "group name: " << meshes[i].name << std::endl;

    meshes[i].numTriangles = *(unsigned short *)ptr;
    ptr += sizeof(unsigned short);
    std::cout << "num triangles: " << meshes[i].numTriangles << std::endl;

    meshes[i].triangleIndices = (unsigned short *)ptr;
    ptr += (sizeof(unsigned short) * meshes[i].numTriangles);

    meshes[i].materialIndex = *(char *)ptr;
    ptr += sizeof(char);
  }

  // read number of materials (2 byte)
  numMaterials = *(unsigned short *)ptr;
  // advance pointer
  ptr += sizeof(unsigned short);
  std::cout << "Number of Materials: " << numMaterials << std::endl;

  // set pointer onto materials
  materials = (MS3D_Material *)ptr;
  // advance pointer
  ptr += (sizeof(MS3D_Material) * numMaterials);

  // read keyframer data
  animationFPS = *(float *)ptr;
  ptr += sizeof(float);
  std::cout << "animationFPS: " << animationFPS << std::endl;
  currentTime = *(float *)ptr; 
  ptr += sizeof(float);
  std::cout << "currentTime: " << currentTime << std::endl;
  totalFrames = *(int *)ptr;
  ptr += sizeof(int);
  std::cout << "totalFrames: " << totalFrames << std::endl;

  // read number of joints
  numJoints = *(unsigned short *)ptr;
  ptr += sizeof(unsigned short);
  std::cout << "numJoints: " << numJoints << std::endl;

  // Then come nNumJoints joints
  // there is a static part in every joint which consists of 
  //
  //byte            flags;                              // SELECTED | DIRTY
  //char            name[32];                           //
  //char            parentName[32];                     //
  //float           rotation[3];                        // local reference matrix
  //float           position[3];
  //
  //word            numKeyFramesRot;                    //
  //word            numKeyFramesTrans;                  //
  //
  // After that, there are numKeyFramesRot
  //
  //typedef struct // 16 bytes
  //{
  //    float           time;                               // time in seconds
  //    float           rotation[3];                        // x, y, z angles
  //} ms3d_keyframe_rot_t;
  //
  // After that, there are numKeyFramesTrans
  //
  //typedef struct // 16 bytes
  //{
  //    float           time;                               // time in seconds
  //    float           position[3];                        // local position
  //} ms3d_keyframe_pos_t;

  MS3D_Joint * joint = NULL;
  joints = new WB_MS3D_Joint[numJoints];
  for (int i = 0; i < numJoints; i++) {
    joint = (MS3D_Joint *)ptr;
    ptr += sizeof(unsigned char);
    ptr += (sizeof(char) * 2 * 32);
    ptr += (sizeof(float) * 2 * 3);
    ptr += (sizeof(unsigned short) * 2);

    /*std::cout << "Joint " << i << " flags: " << joint->flags << std::endl;
    std::cout << "Joint " << i << " name: " << joint->name << std::endl;
    std::cout << "Joint " << i << " parentName: " << joint->parentName << std::endl;
    std::cout << "Joint " << i << " rotation: (" << joint->rotation[0] << "|" << joint->rotation[1] << "|" << joint->rotation[2] << ")" << std::endl;
    std::cout << "Joint " << i << " position: (" << joint->position[0] << "|" << joint->position[1] << "|" << joint->position[2] << ")" << std::endl;
    std::cout << "Joint " << i << " numKeyFramesRot: " << joint->numKeyFramesRot << std::endl;
    std::cout << "Joint " << i << " numKeyFramesTrans: " << joint->numKeyFramesTrans << std::endl;*/

    std::cout << joint->name << " rotation: (" << joint->rotation[0] << "|" << joint->rotation[1] << "|" << joint->rotation[2] << ")" << std::endl;
    std::cout << joint->name << " position: (" << joint->position[0] << "|" << joint->position[1] << "|" << joint->position[2] << ")" << std::endl;

    joints[i].flags = joint->flags;
    memcpy(joints[i].name, joint->name, 32);
    memcpy(joints[i].parentName, joint->parentName, 32);
    joints[i].rotation[0] = joint->rotation[0];
    joints[i].rotation[1] = joint->rotation[1];
    joints[i].rotation[2] = joint->rotation[2];
    joints[i].position[0] = joint->position[0];
    joints[i].position[1] = joint->position[1];
    joints[i].position[2] = joint->position[2];
    joints[i].numKeyFramesRot = joint->numKeyFramesRot;
    joints[i].numKeyFramesTrans = joint->numKeyFramesTrans;

    // copy frame rotations
    if (0 < joint->numKeyFramesRot) {
      joints[i].keyFramesRot = new MS3D_Keyframe_Rot[joint->numKeyFramesRot];
      MS3D_Keyframe_Rot * kfRotPtr = (MS3D_Keyframe_Rot *)ptr;
      for (int j = 0; j < joint->numKeyFramesRot; j++) {
        joints[i].keyFramesRot[j].time = kfRotPtr->time;
        joints[i].keyFramesRot[j].rotation[0] = kfRotPtr->rotation[0];
        joints[i].keyFramesRot[j].rotation[1] = kfRotPtr->rotation[1];
        joints[i].keyFramesRot[j].rotation[2] = kfRotPtr->rotation[2];

        kfRotPtr++;
      }
    } else {
      joints[i].keyFramesRot = NULL;
    }

    // advance pointer
    ptr += (sizeof(MS3D_Keyframe_Rot) * joint->numKeyFramesRot);

    // copy frame translations
    if (0 < joint->numKeyFramesTrans) {
      joints[i].keyFramesTrans = new MS3D_Keyframe_Pos[joint->numKeyFramesTrans];
      MS3D_Keyframe_Pos * kfTransPtr = (MS3D_Keyframe_Pos *)ptr;
      for (int j = 0; j < joint->numKeyFramesTrans; j++) {
        joints[i].keyFramesTrans[j].time = kfTransPtr->time;
        joints[i].keyFramesTrans[j].position[0] = kfTransPtr->position[0];
        joints[i].keyFramesTrans[j].position[1] = kfTransPtr->position[1];
        joints[i].keyFramesTrans[j].position[2] = kfTransPtr->position[2];

        kfTransPtr++;
      }
    } else {
      joints[i].keyFramesTrans = NULL;
    }

    // advance pointer
    ptr += (sizeof(MS3D_Keyframe_Pos) * joint->numKeyFramesTrans);
  }
  joint = NULL;

  // express the joint hierarchie by indices instead of names
  findParentID();

  setupJoints();

  restart();
}

void wbLib::ms3d::MS3DLoader::setupJoints() {
  // This first loop sets the relative matrix to the local 
  // transformation that was stored in the Milkshape 3D file. 
  // The absolute matrix is set to the relative matrix, multiplied by
  // the parent's absolute matrix if there is one. So keep these in 
  // mind - absolute is the matrix to get from (0,0,0) to the joint 
  // following the hierachy, relative matrix is to get from the parent 
  // joint to this joint.

  // 1. create relative matrix from local rotation and translation
  // 2. If joint has a parent, combine absolute matrix from own 
  // relative and parent absolute. If joint has no parent, own absolute
  // equals own relative
  WB_MS3D_Joint * joint = NULL;
	for (int i = 0; i < numJoints; i++)	{
		joint = &joints[i];

		//joint.m_relative.setRotationRadians( joint.m_localRotation );
    //joint->relative.SetRotationEuler(joint->rotation[0], 
    //  joint->rotation[1], joint->rotation[2]);

    // The matrix function needs degrees as parameter values,
    // but ms3d stores radians, so we convert from radians to degrees
    joint->relative.SetRotationEuler(joint->rotation[0] * 180.0f / M_PI,
      joint->rotation[1] * 180.0f / M_PI,
      joint->rotation[2] * 180.0f / M_PI);

		//joint.m_relative.setTranslation( joint.m_localTranslation );
    joint->relative.SetTranslationPart(joint->position);
    //joint->relative.SetTranslation(joint->position);

		if (joint->parentJoint != -1) {
			//joint.absolute.set(joints[joint.parentJoint].absolute.entries);
      memcpy(joint->absolute.entries, 
        joints[joint->parentJoint].absolute.entries, (sizeof(float) * 16));
			//joint.absolute.postMultiply(joint.m_relative);
      joint->absolute *= joint->relative;
		} else {
			//joint.absolute.set(joint.m_relative.getMatrix());
      memcpy(joint->absolute.entries, 
        joint->relative.entries, (sizeof(float) * 16));
    }
	}

	for (int i = 0; i < numVertices; i++)	{
		MS3D_Vertex & vertex = vertices[i];

		if (vertex.boneID != -1) {
			const Matrix4X4f & matrix = joints[vertex.boneID].absolute;

			matrix.InverseTranslateVector3f(vertex.vertex);
			matrix.InverseRotateVector3f(vertex.vertex);
		}
	}
}

/**
 * The restart function is used to set the animation to its original state.
 */
void wbLib::ms3d::MS3DLoader::restart() {
	for (int i = 0; i < numJoints; i++)	{
		joints[i].currentRotationKeyframe = 0;
    joints[i].currentTranslationKeyframe = 0;

		//joints[i].final.set(joints[i].absolute.getMatrix());
    memcpy(joints[i].final.entries, 
        joints[i].absolute.entries, (sizeof(float) * 16));
	}

	timer.reset();
}

void wbLib::ms3d::MS3DLoader::advanceAnimation() {

  bool looping = true;

  // The start is quite simple. the getTime() function 
  // gets the time in milliseconds since the timer was 
  // reset (ie. since the restart function was called). 
  // If the time is past the total time of the animation, 
  // either you set it to 0 and restart the animation 
  // (if the animation is looping), or set it to the 
  // final time of the animation so you get the last frame.
	/*double time = timer.getTime();

	if (time > (totalFrames * animationFPS))	{
		if (looping) {
			restart();
			time = 0;
		} else {
			time = (totalFrames * animationFPS);
    }
	}*/
  //double time = 0.2f;

  // getTime() gives time since last reset() in milliseconds, but
  // we need fractions of a second, so divide it by milliseconds
  // per second (= 1000.0f)
  double time = (timer.getTime() / 1000.0f);
  if (time > ((double)totalFrames / (double)animationFPS)) {
    restart();
    time = 0.0;
  }

  // Now is the time to start looping through all of the 
  // joints and updating the transformations.

	for (int i = 0; i < numJoints; i++)	{
		float transVec[3];
		Matrix4X4f transform;
		int frame;
		WB_MS3D_Joint * joint = &joints[i];

    // Apart from the local variables which we will use, 
    // the first part checks to see if there are indeed 
    // any keyframes for this joint. If not, then there 
    // is nothing to interpolate, so we just set the final 
    // matrix to the absolute matrix and continue to the next joint.
		if (joint->numKeyFramesRot == 0 
      && joint->numKeyFramesTrans == 0)
		{
			
      memcpy(joint->final.entries, 
        joint->absolute.entries, (sizeof(float) * 16));

    //  if (joint->parentJoint == -1) {
			 // memcpy(joint->final.entries, 
    //      joint->absolute.entries,
    //      //joint->relative.entries,
    //      (sizeof(float) * 16));
    //  } else {
    //    memcpy(joint->final.entries, 
    //      joints[joint->parentJoint].final.entries, (sizeof(float) * 16));
    //    joint->final *= joint->relative;
		  //}

			continue;
		}

    // If there are keyframes, then we start with any translation keyframes:
    
    // Starting at the current keyframe, we search forward to see 
    // if we have passed the next keyframe. The while loop ensures 
    // that if the computer is slow, the animation will keep the 
    // correct timing, even if it is a little jerky (or a lot!).
    // So the current keyframe is the one we are moving towards.
		frame = joint->currentTranslationKeyframe;
		while (frame < joint->numKeyFramesTrans
      && (joint->keyFramesTrans[frame].time < time))
		{
			frame++;
		}
		joint->currentTranslationKeyframe = frame;

    // This section below sets up the transVec array to the 
    // appropriate translation vector. If we are at the start 
    // of the animation (this occurs only if there is just one keyframe), 
    // the vector is that first keyframe. If at the end, it is set to 
    // the final keyframe position.
    if (frame == 0) {
			memcpy(transVec, 
        joint->keyFramesTrans[0].position,
        sizeof(float) * 3);
    } else if (frame == joint->numKeyFramesTrans) {
			memcpy(transVec, 
        joint->keyFramesTrans[frame-1].position, 
        sizeof(float) * 3);
    }	else {

      // If neither is the case, the previous frame is found, 
      // and then the vector is interpolated between the two 
      // according to how far it is between them in time.
			//assert( frame > 0 && frame < joint->numKeyFramesTrans );

			const MS3D_Keyframe_Pos& curFrame = joint->keyFramesTrans[frame];
			const MS3D_Keyframe_Pos& prevFrame = joint->keyFramesTrans[frame-1];

			float timeDelta = curFrame.time - prevFrame.time;
			float interpValue = (float)((time - prevFrame.time) / timeDelta);

			transVec[0] = prevFrame.position[0] + ( curFrame.position[0] - prevFrame.position[0] ) * interpValue;
			transVec[1] = prevFrame.position[1] + ( curFrame.position[1] - prevFrame.position[1] ) * interpValue;
			transVec[2] = prevFrame.position[2] + ( curFrame.position[2] - prevFrame.position[2] ) * interpValue; 
		}

    // As above, but for rotation instead. 
    // The difference is at the bottom here. 
    // The second segment is commented out because 
    // it does the same as what the four lines above do. 
    // Sort of. You'll notice the second is basically 
    // equivalent to the interpolation in the transformation.
    // In most cases, this will work. 
    // But there are situations where it won't because of the 
    // nature of Euler angles and something called "Gimbal lock".
    // This is where that Hexapod FAQ is really handy because 
    // it gives a detailed explanation, as does Steve Baker's site 
    // (the address of which eludes me). You can probably get away 
    // with it, but we've got a quaternion class, so we might as well use it!
		frame = joint->currentRotationKeyframe;
		while (frame < joint->numKeyFramesRot 
      && joint->keyFramesRot[frame].time < time )
		{
			frame++;
		}
		joint->currentRotationKeyframe = frame;

    if (frame == 0) {
			//transform.setRotationRadians( joint->keyFramesRot[0].m_parameter );
      transform.SetRotationEuler(joint->keyFramesRot[0].rotation[0] * 180.0f / M_PI,
        joint->keyFramesRot[0].rotation[1] * 180.0f / M_PI,
        joint->keyFramesRot[0].rotation[2] * 180.0f / M_PI);

    }	else if ( frame == joint->numKeyFramesRot ) {
			//transform.setRotationRadians( joint->keyFramesRot[frame-1].m_parameter );
      transform.SetRotationEuler(joint->keyFramesRot[frame-1].rotation[0] * 180.0f / M_PI,
        joint->keyFramesRot[frame-1].rotation[1] * 180.0f / M_PI,
        joint->keyFramesRot[frame-1].rotation[2] * 180.0f / M_PI);
    } else {
			//assert( frame > 0 && frame < joint->numKeyFramesTrans );

			const MS3D_Keyframe_Rot& curFrame = joint->keyFramesRot[frame];
			const MS3D_Keyframe_Rot& prevFrame = joint->keyFramesRot[frame-1];
			
			float timeDelta = curFrame.time - prevFrame.time;
			float interpValue = ( float )(( time - prevFrame.time ) / timeDelta );

			//assert( interpValue >= 0 && interpValue <= 1 );

      //Quaternion qPrev(prevFrame.rotation);
			//Quaternion qCur(curFrame.rotation);
			//Quaternion qFinal(qPrev, qCur, interpValue);
			//transform.setRotationQuaternion(qFinal);

			float rotVec[3];
			rotVec[0] = prevFrame.rotation[0] + (curFrame.rotation[0] - prevFrame.rotation[0]) * interpValue;
			rotVec[1] = prevFrame.rotation[1] + (curFrame.rotation[1] - prevFrame.rotation[1]) * interpValue;
			rotVec[2] = prevFrame.rotation[2] + (curFrame.rotation[2] - prevFrame.rotation[2]) * interpValue;

			//transform.setRotationRadians( rotVec );
      transform.SetRotationEuler(rotVec[0] * 180.0f / M_PI,
        rotVec[1] * 180.0f / M_PI,
        rotVec[2] * 180.0f / M_PI);
		}

    // combine translation and rotation for 
    // this animation delta into a single matrix
		transform.SetTranslationPart(transVec);

    // The transform is constructed from the rotation, 
    // and from the translation vector we found above 
    // using the last line above. Now to do something with it.

		Matrix4X4f relativeFinal(joint->relative);
		//relativeFinal.postMultiply( transform );
    relativeFinal *= transform;

    if (joint->parentJoint == -1) {
			//joint->final.set( relativeFinal.getMatrix());
      memcpy(joint->final.entries, 
        relativeFinal.entries, (sizeof(float) * 16));
    } else {
			//joint->final.set( m_joints[joint->m_parent].final.getMatrix());
      memcpy(joint->final.entries, 
        joints[joint->parentJoint].final.entries, (sizeof(float) * 16));

			//joint->final.postMultiply( relativeFinal );
      joint->final *= relativeFinal;
		}
	}
}

/**
 * Iterates over groups/meshes. For each group/mesh, iterates
 * over vertices. The vertices know the joints they are attached to.
 * The joints contain the final matrix which is changed by the animation.
 * The final matrix is used to position and rotate the vertices
 * 
 * The animations job is to alter the final matrix for the joints,
 * so that it creates the current skeletal position at the current time of 
 * the animation.
 *
 * This means, if you want your model animated, you have to call a 
 * method somewhere, which computes a new final matrix for each 
 * joint and stores this final matrix into the joints. drawGroupsUsingJoints()
 * will then find and reuse the final matrix.
 */
void wbLib::ms3d::MS3DLoader::drawGroupsUsingJoints() {
  glPushMatrix();

  for (int i = 0; i < numMeshes; i++) {

    glBegin(GL_TRIANGLES);
    for (int j = 0; j < meshes[i].numTriangles; j++) {

      int triangleIndex = meshes[i].triangleIndices[j];
      const MS3D_Triangle * tri = &triangles[triangleIndex];

      for (int k = 0; k < 3; k++) {
				int index = tri->vertexIndices[k];

				if (vertices[index].boneID == -1) {
					// same as before
					glNormal3fv(tri->vertexNormals[k]);
				  glTexCoord2f(tri->s[k], tri->t[k]);
          glVertex3fv(vertices[index].vertex);
				}	else {
					// rotate according to transformation matrix
					const Matrix4X4f& final = 
            joints[vertices[index].boneID].final;

					glTexCoord2f(tri->s[k], tri->t[k]);

					//Vector newNormal(pTri->m_vertexNormals[k]);
          //newNormal.transform3(final);
					//newNormal.normalize();
					//glNormal3fv( newNormal.getVector());

					Vector4f newVertex(vertices[index].vertex[0],
            vertices[index].vertex[1],
            vertices[index].vertex[2]);
					//newVertex.transform(final);
          Vector4f result = final * newVertex;
					//glVertex3fv(newVertex.getVector());
          //glVertex3fv(result);
          glVertex3f(result.x, result.y, result.z);
				}

			}
    }
    glEnd();
  }
  glPopMatrix();
}

/**
 * Draws the joints without animation.
 * Draws a glutSolidSphere() at a joints position. The
 * joint positions are loaded from the model file.
 */
void wbLib::ms3d::MS3DLoader::drawJoints() {
  glPushMatrix();
  //glBegin(GL_POINTS);
  /*for (int i = 0; i < numVertices; i++) {
    glVertex3f(vertices[i].vertex[0], vertices[i].vertex[1], vertices[i].vertex[2]);
  }*/
  //glEnd();

  //Vector3f v(0.0f, 0.0f, 0.0f);
	//WB_MS3D_Joint * joint = &joints[0];

  //Vector4f result = joint->absolute * v;
  //Vector3f result = joint->absolute.GetTranslatedVector3f(v);

  for (int i = 0; i < numJoints; i++)	{
    Vector4f v(0.0f, 0.0f, 0.0f, 1.0f);
		WB_MS3D_Joint * joint = &joints[i];

    //Vector3f result = joint->absolute.GetTranslatedVector3f(v);
    Vector4f result = joint->absolute * v;

    glPushMatrix();
      glTranslatef(result.x, result.y, result.z);
      glutSolidSphere(0.5f, 5, 5);
    glPopMatrix();
	}
  
  glPopMatrix();
}

/**
 * Needs a call to advanceAnimation()
 */
void wbLib::ms3d::MS3DLoader::drawJointsAnimated() {
  glPushMatrix();

  for (int i = 0; i < numJoints; i++)	{
    Vector4f v(0.0f, 0.0f, 0.0f, 1.0f);
		WB_MS3D_Joint * joint = &joints[i];

    Vector4f result = joint->final * v;

    glPushMatrix();
      glTranslatef(result.x, result.y, result.z);
      glutSolidSphere(0.5f, 5, 5);
    glPopMatrix();
	}
  
  glPopMatrix();
}

/**
 * The field parentJoint from the WB_MS3D_Joint struct cannot be loaded
 * from the model file, as the hierarchie is not stored by indices into
 * the joint array, but by names of joints. This methode deduces the 
 * parentIDs from the names. It therefore scans the joint array repeatedly
 * for each joint. parentIDs are just indices into the joint array.
 */
void wbLib::ms3d::MS3DLoader::findParentID() {
  if (NULL == joints) {
    return;
  }

  WB_MS3D_Joint * ptr = joints;
  for (int i = 0; i < numJoints; i++) {

    //std::cout << "Joint: " << i << " Parentjointname: " << ptr->parentName << std::endl;

    // empty parent name means that this joint has no parent
    // store a -1 to indicate that fact
    if (0 == strcmp(ptr->parentName, "")) {
      //std::cout << "Joint: " << i << " has no parent" << std::endl;
      ptr->parentJoint = -1;
    } else {

      // search name and store index
      WB_MS3D_Joint * searchPtr = joints;
      for (int j = 0; j < numJoints; j++) {
        if (0 == strcmp(searchPtr->name, ptr->parentName)) {
          ptr->parentJoint = j;
          break;
        }
        // advance pointer
        searchPtr++;
      }
    }

    /*if (-1 == ptr->parentJoint) {
      std::cout << "Joint " << i << " called " << ptr->name << " has no parent" << std::endl;
    } else {
      std::cout << "Joint " << i << " called " << ptr->name << " has " 
        << joints[ptr->parentJoint].name << " as parent" << std::endl;
    }*/

    // advance the pointer
    ptr++;
  }
}

void wbLib::ms3d::MS3DLoader::drawVertices() {
  glPushMatrix();
  glBegin(GL_POINTS);
  for (int i = 0; i < numVertices; i++) {
    glVertex3f(vertices[i].vertex[0], vertices[i].vertex[1], vertices[i].vertex[2]);
  }
  glEnd();
  glPopMatrix();
}

void wbLib::ms3d::MS3DLoader::drawTriangles() {
  glPushMatrix();
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < numTriangles; i++) {
    glVertex3f(vertices[triangles[i].vertexIndices[0]].vertex[0],
      vertices[triangles[i].vertexIndices[0]].vertex[1],
      vertices[triangles[i].vertexIndices[0]].vertex[2]);

    glVertex3f(vertices[triangles[i].vertexIndices[1]].vertex[0],
      vertices[triangles[i].vertexIndices[1]].vertex[1],
      vertices[triangles[i].vertexIndices[1]].vertex[2]);

    glVertex3f(vertices[triangles[i].vertexIndices[2]].vertex[0],
      vertices[triangles[i].vertexIndices[2]].vertex[1],
      vertices[triangles[i].vertexIndices[2]].vertex[2]);
  }
  glEnd();
  glPopMatrix();
}

void wbLib::ms3d::MS3DLoader::drawGroups() {
  glPushMatrix();

  for (int i = 0; i < numMeshes; i++ ) {

    glBegin(GL_TRIANGLES);
    for (int j = 0; j < meshes[i].numTriangles; j++) {

      int triangleIndex = meshes[i].triangleIndices[j];
      const MS3D_Triangle * tri = &triangles[triangleIndex];

      for (int k = 0; k < 3; k++) {
				int index = tri->vertexIndices[k];

				glNormal3fv(tri->vertexNormals[k]);
				glTexCoord2f(tri->s[k], tri->t[k]);
        glVertex3fv(vertices[index].vertex);
			}

      /*glVertex3f(vertices[triangles[i].vertexIndices[0]].vertex[0],
        vertices[triangles[i].vertexIndices[0]].vertex[1],
        vertices[triangles[i].vertexIndices[0]].vertex[2]);

      glVertex3f(vertices[triangles[i].vertexIndices[1]].vertex[0],
        vertices[triangles[i].vertexIndices[1]].vertex[1],
        vertices[triangles[i].vertexIndices[1]].vertex[2]);

      glVertex3f(vertices[triangles[i].vertexIndices[2]].vertex[0],
        vertices[triangles[i].vertexIndices[2]].vertex[1],
        vertices[triangles[i].vertexIndices[2]].vertex[2]);*/
    }
    glEnd();
  }
  glPopMatrix();
}

void wbLib::ms3d::MS3DLoader::cleanUp() {
	if (NULL != buffer) {
		delete [] buffer;
		buffer = NULL;
	}
  vertices = NULL;
  triangles = NULL;
  if (NULL != meshes) {
    delete [] meshes;
    meshes = NULL;
  }
  materials = NULL;
  

  numVertices = 0;
  numTriangles = 0;
  numMeshes = 0;
  numMaterials = 0;

  animationFPS = 0.0f;
	currentTime = 0.0f;
	totalFrames = 0;  

  if (joints != NULL) {
    for (int i = 0; i < numJoints; i++) {
      // cleanUp frame rotations
      if (NULL != joints[i].keyFramesRot) {
        delete [] joints[i].keyFramesRot;
        joints[i].keyFramesRot = NULL;
      }
      // cleanUp frame translations
      if (NULL != joints[i].keyFramesTrans) {
        delete [] joints[i].keyFramesTrans;
        joints[i].keyFramesTrans = NULL;
      }
    }
    delete [] joints;
    joints = NULL;
  }

  numJoints = 0;
}