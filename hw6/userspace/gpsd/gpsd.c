#include "gpsd.h"
void daemon_mode(void)
{
	pid_t pid;

	pid = fork();
	if (pid != 0)
		printf("pid:%d\n", pid);
	if (pid < 0)
		exit(EXIT_FAILURE);
	if (pid > 0)
		exit(EXIT_SUCCESS);
	if (setsid() == -1)
		exit(EXIT_FAILURE);
	chdir("/");
	umask(0);
}
/* get data from files */
int getdata(void)
{
	FILE *fp;
	double lat, lon, acc;

	fp = fopen(GPS_LOCATION_FILE, "r");
	if (fp == NULL) {
		printf("open file error\n");
		return -1;
	}
	if (fscanf(fp, "%lf", &lat) == -1) {
		printf("read lat error\n");
		return -1;
	}
	if (fscanf(fp, "%lf", &lon) == -1) {
		printf("read lat error\n");
		return -1;
	}
	if (fscanf(fp, "%lf", &acc) == -1) {
		printf("read lat error\n");
		return -1;
	}
	struct gps_location tmp;

	tmp.latitude = lat;
	tmp.longitude = lon;
	tmp.accuracy = acc;
	printf("%.2f %.2f %.2f\n", tmp.latitude, tmp.longitude, tmp.accuracy);
	printf("%d\n", set_gps_location(&tmp));
	fclose(fp);
	return 0;
}
int main(int argc, char *argv[])
{
	daemon_mode();
	while (1) {
		/* get data and store every 1 second */
		getdata();
		sleep(1);
	}
	return 0;
}
