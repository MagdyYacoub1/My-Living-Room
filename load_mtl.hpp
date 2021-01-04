#include <unordered_map> 
#include <string>
#include <stdio.h>
#include <string.h>
#pragma warning(disable : 4996)

struct mtl {
	float Ns;
	float Ka[4];
	float Kd[4];
	float Ks[4];
	float Ke[4];
	float Ni;
	float d;
	std::string map_Kd = "no_mat.bmp";
};

std::unordered_map<std::string, mtl*> mtl_map;

void load_mtl()
{
//	FILE *f = fopen("space-shuttle-orbiter.mtl", "r");
	FILE *f = fopen("mansion.mtl", "r");
	//FILE *f = fopen("panel.mtl", "r");
//	FILE *f = fopen("panel_clipped.mtl", "r");
	//FILE *f = fopen("rug.mtl", "r");

	if (!f) {
		printf("failed to open obj model.\n");
		exit(1);
	}
	char line[128];
	memset(line, 0, 128);
	
	while(1) {
		int res = fscanf(f, "%s", line);
		if (res == EOF) {
			break;
		}

		if (strcmp(line, "newmtl") == 0) {
			char mat_name[32];
			memset(mat_name, 0, 32); 
			fscanf(f, "%s\n", mat_name);
			mtl_map[mat_name] = new mtl;

			while(1) {
				char *ret = fgets(line, 128, f);

				/* end of material is empty space */
				if (*line == '\n' || ret == NULL)
					break;

				/* parse material */
				char first[128];
				memset(first, 0, 128);
				sscanf(line, "%s", first);
				printf("%s\n", first);
				
				if (strcmp(first, "Ns") == 0) {
					sscanf(line, "%s %f\n", first, &(mtl_map[mat_name]->Ns));
					printf("%f\n", mtl_map[mat_name]->Ns);
				}

				if (strcmp(first, "Ka") == 0) {
					sscanf(line, "%s %f %f %f\n", first, 
						&(mtl_map[mat_name]->Ka[0]), &(mtl_map[mat_name]->Ka[1]), &(mtl_map[mat_name]->Ka[2]));
					mtl_map[mat_name]->Ka[3] = 1.f;
					printf("%f %f %f\n", mtl_map[mat_name]->Ka[0], mtl_map[mat_name]->Ka[1], mtl_map[mat_name]->Ka[2]);
				}
				if (strcmp(first, "Kd") == 0) {
					sscanf(line, "%s %f %f %f\n", first, 
						&(mtl_map[mat_name]->Kd[0]), &(mtl_map[mat_name]->Kd[1]), &(mtl_map[mat_name]->Kd[2]));
					mtl_map[mat_name]->Kd[3] = 1.f;
					printf("%f %f %f\n", mtl_map[mat_name]->Kd[0], mtl_map[mat_name]->Kd[1], mtl_map[mat_name]->Kd[2]);
				}
				if (strcmp(first, "Ks") == 0) {
					sscanf(line, "%s %f %f %f\n", first, 
						&(mtl_map[mat_name]->Ks[0]), &(mtl_map[mat_name]->Ks[1]), &(mtl_map[mat_name]->Ks[2]));
					mtl_map[mat_name]->Ks[3] = 1.f;
					printf("%f %f %f\n", mtl_map[mat_name]->Ks[0], mtl_map[mat_name]->Ks[1], mtl_map[mat_name]->Ks[2]);
				}
				if (strcmp(first, "Ke") == 0) {
					sscanf(line, "%s %f %f %f\n", first, 
						&(mtl_map[mat_name]->Ke[0]), &(mtl_map[mat_name]->Ke[1]), &(mtl_map[mat_name]->Ke[2]));
					mtl_map[mat_name]->Ke[3] = 1.f;
					printf("%f %f %f\n", mtl_map[mat_name]->Ke[0], mtl_map[mat_name]->Ke[1], mtl_map[mat_name]->Ke[2]);
				}
				if (strcmp(first, "Ni") == 0) {
					sscanf(line, "%s %f\n", first, &(mtl_map[mat_name]->Ni));
					printf("%f\n", mtl_map[mat_name]->Ni);
				}
				if (strcmp(first, "d") == 0) {
					sscanf(line, "%s %f\n", first, &(mtl_map[mat_name]->d));
					printf("%f\n", mtl_map[mat_name]->d);
				}
				if (strcmp(first, "map_Kd") == 0) {
					char buf[1024];
					memset(buf, 0, 1024);
					sscanf(line, "%s %s\n", first, buf);
					mtl_map[mat_name]->map_Kd = buf;
					printf("%s\n", (mtl_map[mat_name]->map_Kd).c_str());
				}
			}
		}
	}
}
