#include "file_loc.h"

char *map_format = "http://maps.google.com/?q=%.7f,%.7f";
int main(int argc, char **argv)
{
	char buffer[100];
	struct gps_location loc;
	int ret;

	if (argc != 2) {
		printf("usage: ./file_loc path\n");
		return 1;
	}
	ret = get_gps_location(argv[1], &loc);
	if (ret < 0) {
		printf("get file info error %d\n", ret);
		return 1;
	}
	printf("file info for %s\n", argv[1]);
	printf("latitude:%.7f\n", loc.latitude);
	printf("longitude:%.7f\n", loc.longitude);
	printf("accuracy:%.7f\n", loc.accuracy);
	printf("age:%d\n", ret);
	sprintf(buffer, map_format, loc.latitude, loc.longitude);
	printf("map url:%s\n", buffer);
	return 0;
}
