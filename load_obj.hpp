#include <vector>
#include <string.h>

struct vertex {
	float x;
	float y;
	float z;
};

struct tex_coords {
	float u;
	float v;
};

struct normal {
	float x;
	float y;
	float z;
};

struct fv {
	int v;
	int t;
	int n;
};

struct fm {
	std::string mat;
	std::vector<std::vector<fv>> obj_f;
	std::string object;
};

/* file parsing storage */
std::vector<vertex> obj_v;
std::vector<tex_coords> obj_vt;
std::vector<normal> obj_vn;
std::vector<fm> obj_fm;

/* opengl array pointer storage */
struct vtnm {
	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<float> normals;
	std::string material;
	std::string object;
};

std::vector<vtnm> object_list;
std::unordered_map<std::string, int> obj_map;

void build_vtnf()
{
//	FILE *f = fopen("space-shuttle-orbiter.obj", "r");
	FILE *f = fopen("mansion.obj", "r");
	//FILE *f = fopen("panel.obj", "r");
	//FILE *f = fopen("panel_clipped.obj", "r");
	//FILE *f = fopen("rug.obj", "r");
	
	char obj_name[1024];

	while(1) {

		/* read the first space delimted string */
		char line[128];
		int res = fscanf(f, "%s", line);
		if (res == EOF) {
			break;
		}

		/* handle the vertex case */
		if (strcmp(line, "v") == 0) {
			vertex v;
			fscanf(f, "%f %f %f\n", &v.x, &v.y, &v.z);
			obj_v.push_back(v);
		}

		/* handle the texture case */
		if (strcmp(line, "vt") == 0) {
			tex_coords t;
			fscanf(f, "%f %f\n", &t.u, &t.v);
			obj_vt.push_back(t);
		}

		/* handle the normal case */
		if (strcmp(line, "vn") == 0) {
			normal n;
			fscanf(f, "%f %f %f\n", &n.x, &n.y, &n.z);
			obj_vn.push_back(n);
		}

		/* add material to fm list */
		std::string current_mat;
		if (strcmp(line, "usemtl") == 0) {
			char buf[32];
			memset(buf, 0, 32);
			fscanf(f, "%s\n", buf);

			fm temp;
			obj_fm.push_back(temp);
			obj_fm.back().mat = buf;
		}

		/* handle the face case */
		if (strcmp(line, "f") == 0) {
			fv face[3];
			fscanf(f, "%d/%d/%d %d/%d/%d %d/%d/%d\n", 
					&face[0].v, &face[0].t, &face[0].n,
					&face[1].v, &face[1].t, &face[1].n,
					&face[2].v, &face[2].t, &face[2].n);

			/* 
			 * create the index list for the face.
			 */
			std::vector<fv> temp;
			temp.push_back(face[0]);
			temp.push_back(face[1]);
			temp.push_back(face[2]);

			/* add the face to the current face-material pair */
			obj_fm.back().obj_f.push_back(temp);

			/* add the object name */
			obj_fm.back().object = obj_name;
		}

		if (strcmp(line, "o") == 0) {
			memset(obj_name, 0, 1024);
			fscanf(f, "%s\n", obj_name);
		}
	}
	fclose(f);

#ifdef DEBUG
	/* print faces */

	/* loop through each object */
	for (int i=0; i<obj_fm.size(); i++) {
		/* loop throught each face */
		for (int j=0; j<obj_fm[i].obj_f.size(); j++) {
			/* loop through each vertex and print */
			for (int k=0; k<obj_fm[i].obj_f[j].size(); k++) {
				std::cout << obj_fm[i].obj_f[j][k].v << "/" << obj_fm[i].obj_f[j][k].t << "/" << obj_fm[i].obj_f[j][k].n << " ";
			}
			std::cout << std::endl;
		}
		
	}

	/* print vertices*/
	for (int i=0; i<obj_v.size(); i++) {
		std::cout << obj_v[i].x << " " << obj_v[i].y << " " << obj_v[i].z << std::endl;
	}

	/* print tex coords */
	for (int i=0; i<obj_vt.size(); i++) {
		std::cout << obj_vt[i].u << " " << obj_vt[i].v << std::endl;
	}
#endif
}



void build_arrays()
{
	/*
	 * loop through each face-material list
	 */
	for (int k=0; k<obj_fm.size(); k++) {
		vtnm temp_object;
		temp_object.material = obj_fm[k].mat;
		temp_object.object = obj_fm[k].object;
		obj_map[obj_fm[k].object] = k;	
		std::cout << temp_object.material;
		/*
		 * loop through each face.
		 */
		for (int i=0; i<obj_fm[k].obj_f.size(); i++) {
			/*
			 * loop through each faces vertex
			 */
			for (int j=0; j<3; j++) {
				/* fetch the data pointed to by the index */
				vertex temp_v = obj_v[obj_fm[k].obj_f[i][j].v - 1];
				tex_coords temp_t = obj_vt[obj_fm[k].obj_f[i][j].t - 1];
				normal temp_n = obj_vn[obj_fm[k].obj_f[i][j].n - 1];

				/* add the data to the arrays */
				temp_object.vertices.push_back(temp_v.x);
				temp_object.vertices.push_back(temp_v.y);
				temp_object.vertices.push_back(temp_v.z);

				temp_object.uvs.push_back(temp_t.u);
				temp_object.uvs.push_back(temp_t.v);

				temp_object.normals.push_back(temp_n.x);
				temp_object.normals.push_back(temp_n.y);
				temp_object.normals.push_back(temp_n.z);
			}
		}
		object_list.push_back(temp_object);
	}
#ifdef DEBUG
#endif
}

