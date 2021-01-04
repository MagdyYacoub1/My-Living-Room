struct texture_image {
	int width;
	int height;
	int size;
	int bit_layout;
	int data_offset;
	char *data;
};

std::vector<texture_image> images;

void load_bmp()
{
	/* TODO:  load the header into a struct instead */
	char header[0x36];	/* BMP header is 0x36 bytes long */

	for (int i=0; i<object_list.size(); i++) {
		memset(header, 0, 0x36);
		texture_image temp;

		printf("opening texture for material: %s\n", object_list[i].material.c_str());
		printf("opening texture: %s\n", mtl_map[object_list[i].material]->map_Kd.c_str());
		FILE *f = fopen(mtl_map[object_list[i].material]->map_Kd.c_str(), "rb");
		if (f != NULL) {
			printf("open success: %s\n", mtl_map[object_list[i].material]->map_Kd.c_str());
		} else {
			printf("Image Load failure\n");
			f = fopen("no_mat.bmp", "rb");
		}


		fread(header, 1, 54, f);

		temp.size = 	*(int*)&(header[0x22]);
		temp.width = 	*(int*)&(header[0x12]);
		temp.height = 	*(int*)&(header[0x16]);
		temp.data_offset = *(int*)&(header[0xa]);

		printf("\033[31mdata_offset: %d\033[0m\n", temp.data_offset);

		short bit_size = *(short*)&(header[0x1c]);
		if (bit_size == 24) {
			temp.bit_layout = GL_BGR_EXT;
		} else if (bit_size == 32) {
			temp.bit_layout = GL_BGRA_EXT;
		} else {
			printf("Error, unsupported bit size\n");
		}


		/*
		 * TODO: WRONG: Read the image data into a buffer.  For .bmp, the data
		 * begins at offset 0x36, which is right after the header.
		 */
		temp.data = new char [temp.size];
		fseek(f, temp.data_offset, SEEK_SET);
		fread(temp.data, 1, temp.size, f);

		images.push_back(temp);


		fclose(f);
	}
}
