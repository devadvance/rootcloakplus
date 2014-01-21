#include "substrate.h"
#include <android/log.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdarg.h>

#define DEBUG_TAG "RootCloakPlus"
#define INFO_TAG "RootCloakPlus"
#define NUM_BLACKLIST 45
#define NUM_SU_LIST 5
#define NUM_KEYWORD_LIST 20
#define NUM_CMD_LIST 2
#define NUM_STAT_LIST 8
#define GREPLIST_LENGTH 53

//#define TURNOFFDEBUG 1
//#define TURNOFFINFO 1

#ifdef TURNOFFDEBUG
#define LOGD(...)
#else
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, __VA_ARGS__);
#endif

#ifdef TURNOFFINFO
#define LOGI(...)
#else
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, INFO_TAG, __VA_ARGS__)
#endif

MSConfig(MSFilterExecutable, "/system/bin/app_process")

static FILE * (*s_orig_fopen) ( const char * filename, const char * mode );
static int (*s_orig_access)(const char *pathname, int mode);
static int (*s_orig_execve)(const char *filename, char *const argv[],char *const envp[]);
static int (*s_orig_stat)( const char *path, struct stat *buf );
static int (*s_orig_system)(const char *command);
static struct dirent * (*s_orig_readdir)(DIR *dirp);
static int (*s_orig_open)(const char *path, int oflags, ... );

const char * greplist[] = { "|", "grep", "-v", "supersu", "|", "grep", "-v", "superuser", "|", "grep", "-v", "Superuser",
						"|", "grep", "-v", "noshufou", "|", "grep", "-v", "xposed", "|", "grep", "-v", "rootcloak",
						"|", "grep", "-v", "chainfire", "|", "grep", "-v", "titanium", "|", "grep", "-v", "Titanium",
						"|", "grep", "-v", "substrate", "|", "grep", "-v", "greenify", "|", "grep", "-v", "root", "|", "grep", "-v", "su", NULL};
const char grepstring[] = " | grep -v supersu | grep -v superuser | grep -v Superuser | grep -v noshufou | grep -v xposed | grep -v rootcloak | grep -v chainfire | grep -v titanium | grep -v Titanium | grep -v substrate | grep -v greenify | grep -v root | grep -v su";


const char * shprefix[] = { "sh", "-c" };

const char* blacklist[] = { "com.fde.DomesticDigitalCopy",
		"com.directv.application.android.go.production",
		"com.res.bby",
		"dk.excitor.dmemail",
		"com.BHTV",
		"com.bradfordnetworks.bma",
		"com.apriva.mobile.bams",
		"com.apriva.mobile.aprivapay",
		"pl.pkobp.iko",
		"au.com.auspost",
		"com.rogers.citytv.phone",
		"com.zenprise",
		"net.flixster.android",
		"com.starfinanz.smob.android.sfinanzstatus",
		"com.ovidos.yuppi",
		"klb.android.lovelive",
		"com.incube.epub",
		"com.airwatch.androidagent",
		"com.zappware.twintv.d3",
		"com.starfinanz.mobile.android.pushtan",
		"com.stofa.webtv",
		"com.barclays.android.barclaysmobilebanking",
		"com.bskyb.skygo",
		"com.hanaskcard.rocomo.potal",
		"com.hanabank.ebk.channel.android.hananbank",
		"com.ahnlab.v3mobileplus",
		"com.good.android.gfe",
		"it.phoenixspa.inbank",
		"dk.tv2.tv2play",
		"com.enterproid.divideinstaller",
		"com.isis.mclient.verizon.activity",
		"com.isis.mclient.atnt.activity",
		"com.isis.mclient.atnt.activity",
		"be.telenet.yelo",
		"no.rdml.android.mobiletv",
		"uk.co.barclays.barclayshomeowner",
		"com.mcafee.apps.emmagent",
		"com.virginmedia.tvanywhere",
		"com.amis.mobiatv",
		"it.telecomitalia.cubovision",
		"nl.ziggo.android.tv",
		"com.orange.fr.ocs",
		"com.adb.android.app.iti",
		"com.mobileiron",
		"be.belgacom.mobile.tveverywhere" };

const char* su_list[] = { "/system/xbin/su",
		"/system/bin/su",
		"/sbin/su",
		"/system/su",
		"su" };

const char* keyword_list[] { "supersu",
		"superuser",
		"Superuser",
		"noshufou",
		"chainfire",
		"substrate",
		"busybox",
		"rootcloak!",
		"SUDaemon",
		"daemonsu",
		".su",
		"su_daemon",
		"packages.list",
		"liblog!",
		"rootcloak",
		"xposed",
		"amphoras",
		"app_process.orig",
		".tmpsu",
		"-ROOTCLOAK"};

const char* cmd_blacklist[] { "su",
		"busybox" ,
		"sh"};

const char* stat_list[] { "su",
		"busybox" ,
		"app_process.orig",
		"dexdump",
		"su",
		".tmpsu",
		"daemonsu",
		".su" };

char **blacklist_entries = NULL;
const char blacklist_filename[] = "/data/data/com.devadvance.rootcloakplus/blacklist-ROOTCLOAK";
int blacklist_entry_count;


/*
 * Returns the number of characters read, including NULL terminator
 * Uses /r as the newline indicator, also separates based on NULL
 * DOES NOT include the newline character!!!
 * This more or less replicates gets, but works with an fd instead of a FILE *
 * Also, it works character by character, probably inefficient but this can be fixed later ;)
 */
int custom_read_line ( char * str, int num, int fd ) {
	if (fd <= 0) {
		return -1;
	}
	if (num < 1 || str == NULL) { // Requires a buffer of at least size 1, and not a NULL pointer!
		return -2;
	}
	char buffer[1]; // Maybe read 32 bytes at a time? We'll see later on

	int bytesRead = 0;
	int position;

	for (position = 0; position < num - 2; position++) {
		bytesRead = read(fd, buffer, 1);
		if (bytesRead > 0) {
			if (buffer[0] == '\n') {
				str[position] = '\0';
				return position + 1;
			} else if (buffer[0] == '\0') {
				str[position] = '\0';
				return position + 1;
			} else {
				str[position] = buffer[0];
			}
		} else {
			return position;
		}
	}
	LOGD("custom_read_line: line: [%s]", str);
	str[num - 1] = '\0';
	return position;
}

/*
 * Uses fopen, bad!
 */
void load_blacklist_entries_old() {
	blacklist_entry_count = 0;
	char line[256]; // Holds the line currently being read
	FILE *blacklist_file;
	if (!(blacklist_file = s_orig_fopen(blacklist_filename, "r"))) { // Open the file to read from
		LOGD("load_blacklist_entries: Error opening blacklist_file. filename: [%s], error: [%s]", blacklist_filename, strerror(errno));
		blacklist_entries = blacklist;
		blacklist_entry_count = NUM_BLACKLIST;
	} else if (fgets(line, 256, blacklist_file) != NULL) {
		line[255] = 0; // Make sure its null terminated
		blacklist_entry_count = atoi(line);
		LOGI("atoi: [%s]", line);

		if (blacklist_entry_count > 0) {

			blacklist_entries = malloc(blacklist_entry_count * sizeof(char*));
			for (int i = 0; i < blacklist_entry_count; i++) {
				blacklist_entries[i] = malloc((256) * sizeof(char)); // yeah, I know sizeof(char) is 1, but to make it clear...
			}

			int counter = 0;
			while ( (fgets (blacklist_entries[counter], 256, blacklist_file) != NULL) && (counter < blacklist_entry_count) ) {
				char *pos;
				if ((pos=strchr(blacklist_entries[counter], '\n')) != NULL) {
				    *pos = '\0';
				}

				LOGI("load_blacklist_entries: Loaded: [%s]", blacklist_entries[counter]);

				counter++;
			}
		} else {
			blacklist_entries = blacklist;
			blacklist_entry_count = NUM_BLACKLIST;
		}

		fclose(blacklist_file);
	} else {
		fclose(blacklist_file);
		blacklist_entries = blacklist;
		blacklist_entry_count = NUM_BLACKLIST;
	}
}

/*
 * Uses open, good!
 */
void load_blacklist_entries() {
	blacklist_entry_count = 0;
	char line[256]; // Holds the line currently being read
	int blacklist_fd;
	blacklist_fd = s_orig_open(blacklist_filename, O_RDONLY);
	if (blacklist_fd <= 0) { // Open the file to read from
		LOGD("load_blacklist_entries: Error opening blacklist_file. filename: [%s], error: [%s]", blacklist_filename, strerror(errno));
		blacklist_entries = blacklist;
		blacklist_entry_count = NUM_BLACKLIST;
	} else if (custom_read_line(line, 256, blacklist_fd) > 0) {
		line[255] = 0; // Make sure its null terminated
		blacklist_entry_count = atoi(line);
		LOGI("atoi: [%s]", line);

		if (blacklist_entry_count > 0) {

			blacklist_entries = malloc(blacklist_entry_count * sizeof(char*));
			for (int i = 0; i < blacklist_entry_count; i++) {
				blacklist_entries[i] = malloc((256) * sizeof(char)); // yeah, I know sizeof(char) is 1, but to make it clear...
			}

			int counter = 0;
			while ( (custom_read_line(blacklist_entries[counter], 256, blacklist_fd) > 0) && (counter < blacklist_entry_count) ) {
				char *pos;
				if ((pos=strchr(blacklist_entries[counter], '\n')) != NULL) {
				    *pos = '\0';
				}

				LOGI("load_blacklist_entries: Loaded: [%s]", blacklist_entries[counter]);

				counter++;
			}
		} else {
			blacklist_entries = blacklist;
			blacklist_entry_count = NUM_BLACKLIST;
		}

		close(blacklist_fd);
	} else {
		close(blacklist_fd);
		blacklist_entries = blacklist;
		blacklist_entry_count = NUM_BLACKLIST;
	}
}




// "daemonsu",
int is_process_on_blacklist(const char * process_name) {
	if (blacklist_entries == NULL) {
		load_blacklist_entries();
	}
	int i;
	for (i = 0; i < blacklist_entry_count; i++) {
		if (strcmp (process_name, blacklist_entries[i]) == 0) {
			return 1;
		}
	}

	return 0;
}

int is_path_on_su_list(const char * path) {
	int i;
	for (i = 0; i < NUM_SU_LIST; i++) {
		if (strcmp (path, su_list[i]) == 0) {
			return 1;
		}
	}

	return 0;
}

int is_path_on_stat_list(const char * path) {
	int i;
	for (i = 0; i < NUM_STAT_LIST; i++) {
		if (strcmp (path, stat_list[i]) == 0) {
			return 1;
		}
	}

	return 0;
}

int is_keyword_in_string(const char * theString) {
	int i;
	for (i = 0; i < NUM_KEYWORD_LIST; i++) {
		char * tempPChar;
		tempPChar = strstr (theString, keyword_list[i]);
		if (tempPChar != NULL) {
			return 1;
		}
	}

	return 0;
}

int ends_with(const char *str, const char *suffix){
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

// Returns 0 if the end of the string does not match any
int check_end_of_cmd_string(const char * cmd_string) {
	int i;
	for (i = 0; i < NUM_CMD_LIST; i++) {
		if (ends_with(cmd_string, cmd_blacklist[i]) == 1) {
			return 1;
		}
	}

	return 0;
}

int is_cmd_in_blacklist(const char * cmd_string) {
	int i;
		for (i = 0; i < NUM_CMD_LIST; i++) {
			if (strcmp (cmd_string, cmd_blacklist[i]) == 0) {
				return 1;
			}
		}

		return 0;
}



std::string get_selfpath() {
    char buff[1024];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    } else {
     return "";
    }
}

/*
 * This one uses fopen, which we can no longer use.
 */
void get_process_name_old(char *buffer, size_t len) {
	FILE* status = fopen( "/proc/self/cmdline", "r" );
	if (status != NULL) {
		fscanf ( status, "%s", buffer );
		fclose (status);
	} else {
		buffer = "NONAME";
		buffer[6] = '\0';
	}
}

/*
 * Uses open.
 * Need to pass in a buffer of at least size 7 please!
 */
void get_process_name(char *buffer, size_t len) {
	//char buf[4096]; // Holds the data in between reading and writing
	int infile, bytesRead;

	infile = s_orig_open("/proc/self/cmdline", O_RDONLY);
	if(infile > 0) {
		bytesRead = read(infile, buffer, len);
		close(infile);
	} else {
		LOGI("read_copy_file: Error opening input: %s", strerror(errno));
		buffer = "NONAME";
		buffer[6] = '\0';
	}
}


/*
 * Perform a open/read/write based copy. Avoids using stdio.
 * Returns 0 if successful, and a positive int if not.
 */
int read_copy_file(const char * filename, const char * new_filename) {
	char buf[4096]; // Holds the data in between reading and writing
	int outfile, infile, bytesRead;

	if((outfile = s_orig_open(new_filename, O_CREAT | O_APPEND | O_RDWR, 0777))==-1){
		LOGD("read_copy_file: Error opening output: %s", strerror(errno));
		return 1;
	}

	infile = s_orig_open(filename, O_RDONLY);
	if(infile > 0) {
		bytesRead = read(infile, buf, 4096);
		while (bytesRead > 0) {
			write(outfile, buf, bytesRead);
			bytesRead = read(infile, buf, 4096);
		}
		LOGD("read_copy_file: Done copying.");
		close(infile);
	} else {
		LOGD("read_copy_file: Error opening input: %s", strerror(errno));
		close(outfile);
		return 2;
	}
	close(outfile);
	return 0;
}

/*
 * Gets the cache dir for the current app process. Based on process name.
 * Returns 0 if successful, and a positive int if not.
 */
int get_cache_path(char *cache_path, size_t len) {
	char process_name[512];
	get_process_name(process_name, 512);

	if (strstr(process_name, "NONAME") == NULL) {
		int snprintf_return = snprintf(cache_path, 512, "/data/data/%s/cache/", process_name);
		if (snprintf_return >= 0) {
			LOGD("get_cache_path: Obtained cache path: [%s]", cache_path);
			return 0; // Return 0 if there was no issue
		} else {
			LOGD("get_cache_path: Error with snprintf: %s", strerror(errno));
			return 1; // Return 1 if there was an issue with snprintf
		}
		return 0;
	} else {
		cache_path = NULL;
		LOGD("get_cache_path: Error getting process name.")
		return 2;
	}

}

//http://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
/*
 * Generates a random c string of length len.
 */
void gen_random(char *s, size_t len) {
	static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}

/*
 * Generates a random filename within the cache dir for the current app process.
 * Returns 0 if successful, and 1 if not.
 */
int generate_random_cache_filename(const char * random_cache_filename, size_t filename_size, size_t random_name_size) {
	char random_name[random_name_size]; // Generate a random name
	char cache_path[filename_size]; // Holds the cachepath

	int get_cache_path_return = get_cache_path(cache_path, filename_size); // Get the cache_path for the current app process

	if (get_cache_path_return != 0) {
		return 2; // Error getting cache path
	}

	gen_random(random_name, 16); // Generate a random c string
	int snprintf_return = snprintf(random_cache_filename, filename_size, "%s%s-ROOTCLOAK", cache_path, random_name); // Combine the cache_path, random c string, and a suffix
	if (snprintf_return >= 0) {
		LOGD("generate_random_cache_filename: Generated random cache filename: %s", random_cache_filename);
		return 0; // Return 0 if there was no issue
	} else {
		LOGD("generate_random_cache_filename: Error with snprintf: %s", strerror(errno));
		return 1; // Return 1 if there was an issue
	}
}

/*
 * This performs filtered duplication of a file on a line-by-line basis. Currently it just removes the entire line if it has a matching keyword.
 * NOT FOR USE WITH MAGIC FILES. USE duplicate_magic_file INSTEAD!
 * Returns 0 if successful, and a positive int if not.
 */
int filtered_duplicate_file_old(const char * filename, const char * new_filename) {
	char random_cache_filename[512]; // Holds the random cache filename that gets generated
	FILE *infile;
	FILE *outfile;
	char line[256]; // Holds the line currently being read
	char modifiedLine[256]; // FOR FUTURE USE. Holds modified version of line being read

	generate_random_cache_filename(random_cache_filename, 512, 16); // Generate a random cache filename

	LOGD("filtered_duplicate_file: filename: [%s], random_cache_filename: [%s]", filename, random_cache_filename);

	if (!(infile = s_orig_fopen(filename, "r"))) { // Open the file to read from
		LOGD("filtered_duplicate_file: Error opening infile. filename: [%s], error: [%s]", filename, strerror(errno));
		return 1; // For now just return the original path that was passed in
	}

	if (!(outfile = s_orig_fopen(random_cache_filename, "w"))) {
		LOGD("filtered_duplicate_file: Error opening outfile. random_cache_filename: [%s], error: [%s]", random_cache_filename, strerror(errno));
		fclose(infile);
		return 2; // For now just return the original path that was passed in
	}

	LOGD("filtered_duplicate_file: Proceeding to copying and filtering...");

	while (fgets (line, 256, infile) != NULL) {
		if (!is_keyword_in_string(line)) { // If the line has no keywords in it, then output it to the new file
			fprintf(outfile,"%s\n", line);
		}
		else {
			LOGI("filtered_duplicate_file: Filtered out line: [%s]", line);
		}
	}

	fclose(infile);
	fclose(outfile);

	strncpy(new_filename, random_cache_filename, 512); // After everything was successful, copy over the filename for usage

	return 0;
}

/*
 * Uses open instead of fopen
 * This performs filtered duplication of a file on a line-by-line basis. Currently it just removes the entire line if it has a matching keyword.
 * NOT FOR USE WITH MAGIC FILES. USE duplicate_magic_file INSTEAD!
 * Returns 0 if successful, and a positive int if not.
 */
int filtered_duplicate_file(const char * filename, const char * new_filename) {
	char random_cache_filename[512]; // Holds the random cache filename that gets generated
	int infile;
	int outfile;
	char line[256]; // Holds the line currently being read
	char modifiedLine[256]; // FOR FUTURE USE. Holds modified version of line being read
	char outputLine[512];
	int bytesToWrite = 0;

	generate_random_cache_filename(random_cache_filename, 512, 16); // Generate a random cache filename

	LOGI("filtered_duplicate_file: filename: [%s], random_cache_filename: [%s]", filename, random_cache_filename);

	infile = s_orig_open(filename, O_RDONLY);
	if (infile <= 0) { // Open the file to read from
		LOGD("filtered_duplicate_file: Error opening infile. filename: [%s], error: [%s]", filename, strerror(errno));
		return 1; // For now just return the original path that was passed in
	}

	outfile = s_orig_open(random_cache_filename, O_CREAT | O_APPEND | O_RDWR, 0777);
	if (outfile <= 0) {
		LOGD("filtered_duplicate_file: Error opening outfile. random_cache_filename: [%s], error: [%s]", random_cache_filename, strerror(errno));
		close(infile);
		return 2; // For now just return the original path that was passed in
	}

	LOGI("filtered_duplicate_file: Proceeding to copying and filtering...");

	while (custom_read_line(line, 256, infile) > 0) {
		if (!is_keyword_in_string(line)) { // If the line has no keywords in it, then output it to the new file
			//dprintf(outfile,"%s\n", line);
			bytesToWrite = sprintf(outputLine, "%s\n", line);
			if (bytesToWrite > 0) {
				write(outfile, outputLine, bytesToWrite); // Don't include NULL terminator
			}
		}
		else {
			//LOGD("filtered_duplicate_file: Filtered out line: [%s]", line);
		}
	}

	close(infile);
	close(outfile);

	strncpy(new_filename, random_cache_filename, 512); // After everything was successful, copy over the filename for usage

	return 0;
}

/*
 * This is for duplicating magic files, such as proc/<pid>/maps and others.
 * They don't show an EOF so you need to do a read() instead of fgets() first.
 * Returns 0 if successful, and a positive int if not.
 */
int duplicate_magic_file(const char * filename, const char * new_filename) {
	int return_value; // Holds the return value from the call to duplicate_file
	char random_cache_filename[512]; // Holds the random cache filename that gets generated

	generate_random_cache_filename(random_cache_filename, 512, 16); // Generate a random cache filename

	LOGD("duplicate_magic_file: filename: [%s], random_cache_filename: [%s]", filename, random_cache_filename);

	if ( (read_copy_file(filename, random_cache_filename) == 0) ) {
		return_value = filtered_duplicate_file(random_cache_filename, new_filename); // Duplicate with filtering
		LOGD("duplicate_magic_file: Removing first cache file.");
		remove(random_cache_filename); // Remove the initial cache file
		return return_value; // Return whatever the call returned
	} else {
		LOGD("duplicate_magic_file: Unknown error with read_copy_file.");
		return 1;
	}
}


struct dirent * my_readdir(DIR * dirp) {
	char buffer[512];
	get_process_name(buffer, 512);

	struct dirent *ep;

	if ((buffer != NULL) && (*buffer != '\0')) { // Check to make sure the buffer is not NULL
		if (is_process_on_blacklist(buffer)) { // If this is a blacklisted process, proceed
			LOGD("NATIVE: READDIR. Process is on the blacklist...");
			if (dirp != NULL) {
				LOGD("NATIVE: READDIR. processname: [%s]", buffer);

				ep = s_orig_readdir(dirp);

				if(ep != NULL) {
					LOGD("NATIVE: READDIR. d_name: [%s]", ep->d_name);
				}

				if ( (ep != NULL) &&  (is_path_on_stat_list(ep->d_name) || is_keyword_in_string(ep->d_name)) ) {
					LOGD("NATIVE: READDIR. d_name is on the blacklist...");
					LOGD("NATIVE: READDIR. processname: [%s] and d_name: [%s]", buffer, ep->d_name);
					LOGD("NATIVE: READDIR. blocking blacklisted d_name...");
					//ep->d_name = "FAKEFILE";
					char random_string[16]; // Generate a random name
					gen_random(random_string, 16); // Generate a random c string
					strcpy(ep->d_name, random_string);
				}

				return ep;
			}
		}
	}


	return s_orig_readdir(dirp);
}

static int my_system (const char *command) {
	char buffer[512];
	get_process_name(buffer, 512);

	if ((buffer != NULL) && (*buffer != '\0')) { // Check to make sure the buffer is not NULL
		if (is_process_on_blacklist(buffer)) { // If this is a blacklisted process, proceed
			LOGD("NATIVE: SYSTEM. Process is on the blacklist...");
			if (command != NULL) {
				LOGD("NATIVE: SYSTEM. processname: [%s] and command: [%s]", buffer, command);
			}
			// If the filename is not NULL, and this is a path we want to hide, do so
			if ( (command != NULL) &&  is_cmd_in_blacklist(command) ) {
				LOGD("NATIVE: SYSTEM. command is on the blacklist...");
				LOGD("NATIVE: SYSTEM. processname: [%s] and command: [%s]", buffer, command);
				LOGD("NATIVE: SYSTEM. blocking blacklisted command...");
				char random_string[16]; // Generate a random name
				const char random_command[32]; // To hold the finished command
				gen_random(random_string, 16); // Generate a random c string

				sprintf(random_command, "%s-ROOTCLOAK", random_string);
				return s_orig_system(random_command);
			}

		}
	}

	return s_orig_system(command);
}

static int my_stat( const char *path, struct stat *buf ) {
	char buffer[512];
	get_process_name(buffer, 512);

	if ((buffer != NULL) && (*buffer != '\0')) { // Check to make sure the buffer is not NULL
		if (is_process_on_blacklist(buffer)) { // If this is a blacklisted process, proceed
			LOGD("NATIVE: STAT. Process is on the blacklist...");
			if (path != NULL) {
				LOGD("NATIVE: STAT. processname: [%s] and path: [%s]", buffer, path);
			}
			// If the filename is not NULL, and this is a path we want to hide, do so
			if ( (path != NULL) &&  (is_path_on_stat_list(path) || is_path_on_su_list(path) || is_keyword_in_string(path) ) ) {
				LOGD("NATIVE: STAT. path is on the blacklist...");
				LOGD("NATIVE: STAT. processname: [%s] and path: [%s]", buffer, path);
				LOGD("NATIVE: STAT. blocking blacklisted path...");
				char random_string[16]; // Generate a random name
				char random_path[32]; // To hold the finished path

				gen_random(random_string, 16); // Generate a random c string

				sprintf(random_path, "%s-ROOTCLOAK", random_string);
				return s_orig_stat(random_path, buf);
			}

		}
	}

	return s_orig_stat(path, buf);

}


static int my_execve(const char *filename, char *const argv[],char *const envp[]) {
	char buffer[512];
	get_process_name(buffer, 512);

	if ((buffer != NULL) && (*buffer != '\0')) { // Check to make sure the buffer is not NULL
		if (is_process_on_blacklist(buffer)) { // If this is a blacklisted process, proceed
			LOGD("NATIVE: EXECVE. Process is on the blacklist...");
			LOGD("NATIVE: EXECVE. ARGV LIST...");
			int i;
			if (filename != NULL) {
				LOGD("NATIVE: EXECVE. filename: [%s]", filename);
			}

			for (i = 0; argv[i] != NULL; i++) {
				LOGD("NATIVE: EXECVE. argv [%d]: [%s]", i, argv[i]);
			}

			// If the filename is not NULL, and this is a path or apk we want to hide, do so
			if (filename != NULL) { // If there is in fact a command coming in
				if ( is_cmd_in_blacklist(argv[0]) ) { // Check first if it's something in the cmd_blacklist
					LOGD("NATIVE: EXECVE. argv[0] is on the blacklist...");
					LOGD("NATIVE: EXECVE. processname: [%s] and filename: [%s]", buffer, filename);
					// If it is on the cmd_blacklist, we just replace it with a random fake command
					char random_string[16]; // Generate a random name
					char random_command[32]; // To hold the finished command
					char random_filename[48];
					gen_random(random_string, 16); // Generate a random c string

					sprintf(random_command, "%s-ROOTCLOAK", random_string);
					sprintf(random_filename, "/system/xbin/%s", random_command);
					char * const command[] = {random_command, NULL};
					return s_orig_execve(random_filename, command, envp);
				} else if ( (strcmp(argv[0], "ps") == 0) || (strcmp(argv[0], "ls") == 0) || ( (strcmp(argv[0], "pm") == 0) && (argv[1] != NULL) && (strcmp(argv[1], "list") == 0) ) ) {
					// If the command is ps, ls, or pm list: We take it, turn it into sh -c *CMD* and add a bunch of greps to it to strip any offending data
					LOGD("NATIVE: EXECVE. argv[0] is ps, pm list, or ls");
					int argv_length = 0; // Length of the exec argv array
					char * temp_arg = 1; // Holds the current arg
					while (temp_arg != NULL) { // NULL indicates the end of the argv exec array
						temp_arg = argv[argv_length];
						argv_length++; // This is okay because we want the LENGTH (i.e. 1-indexed, including the NULL at the end)
					}

					char * totalCommand[GREPLIST_LENGTH + argv_length + 2]; // 2 just in case, really only need 1
					int position = 2;
					totalCommand[0] = shprefix[0]; // Add sh
					totalCommand[1]= shprefix[1]; // Add -c
					for (int k = 0; k < argv_length - 1; k++) { // -1 because we don't want the NULL added to the end
						totalCommand[position] = argv[k];
						position++;
					}
					for (int j = 0; j < GREPLIST_LENGTH; j++) { // Add the mess of greps to the end, including the NULL character
						totalCommand[position] = greplist[j];
						position++;
					}
					for (i = 0; totalCommand[i] != NULL; i++) {
						LOGD("NATIVE: EXECVE. new totalCommand [%d]: [%s]", i, totalCommand[i]);
					}

					return s_orig_execve("/system/bin/sh", totalCommand, envp);
				} else if ((strcmp(argv[0], "sh") == 0) && argv[1] != NULL && argv[2] != NULL) {
					LOGD("NATIVE: EXECVE. checking args for filename: [%s]", filename);
					if (strcmp(argv[2], "su") == 0) {
						// If it is sh -c su, turn su into a random fake command
						LOGD("NATIVE: EXECVE. argv[2] is su");
						char random_string[16]; // Generate a random name
						char random_command[32]; // To hold the finished command

						gen_random(random_string, 16); // Generate a random c string

						sprintf(random_command, "%s-ROOTCLOAK", random_string);

						char * const fakecommand[] = {"sh", "-c", random_command, NULL};
						return s_orig_execve("/system/bin/sh", fakecommand, envp);
					} else if ( (strncmp (argv[2], "ps ", 3) == 0)
							|| (strcmp(argv[2], "ls") == 0)
							|| ( (strcmp(argv[2], "pm") == 0) && (argv[3] != NULL) && (strncmp(argv[3], "list", 4) == 0))
							|| (strncmp(argv[2], "pm list", 7) == 0)
							){
						// If the command is sh -c iwth  ps, ls, or pm list: We take it, turn it into sh -c *CMD* and add a bunch of greps to it to strip any offending data
						LOGD("NATIVE: EXECVE. argv[2] is ps or ls");
						/*int argv_length = 0;
						char * temp_arg = 1;
						while (temp_arg != NULL) {
							temp_arg = argv[argv_length];
							argv_length++;
						}

						char * totalCommand[GREPLIST_LENGTH + argv_length];
						int position;
						for (position = 0; position < argv_length - 1; position++) { // -1 because we don't want the NULL added to the end
							totalCommand[position] = argv[position];
						}
						for (int j = 0; j < GREPLIST_LENGTH; j++) {
							totalCommand[position] = greplist[j];
							position++;
						}
						for (i = 0; totalCommand[i] != NULL; i++) {
							LOGD("NATIVE: EXECVE. new totalCommand [%d]: [%s]", i, totalCommand[i]);
						}*/
						int original_length = strlen(argv[2]);
						int grepstring_length = strlen(grepstring);
						char new_arg [original_length + grepstring_length + 2];
						sprintf(new_arg, "%s%s", argv[2], grepstring);

						char * totalCommand[] = {"sh", "-c", new_arg, NULL};

						for (i = 0; totalCommand[i] != NULL; i++) {
							LOGD("NATIVE: EXECVE. new totalCommand [%d]: [%s]", i, totalCommand[i]);
						}

						return s_orig_execve("/system/bin/sh", totalCommand, envp);
					} else if (strcmp(argv[2], "id") == 0) {
						LOGD("NATIVE: EXECVE. argv[2] is id");
						char * const command[] = {"sh", "-c", "id | grep -v root", NULL};
						return s_orig_execve("/system/bin/sh", command, envp);
					} else if (!ends_with(argv[2], "-ROOTCLOAK")) { // If it has 3 args but doesn't meet the other criteria
						LOGD("NATIVE: EXECVE. Other sh with 3 args");

						/*int argv_length = 0;
						char * temp_arg = 1;
						while (temp_arg != NULL) {
							temp_arg = argv[argv_length];
							argv_length++;
						}

						char * totalCommand[GREPLIST_LENGTH + argv_length];
						int position;
						for (position = 0; position < argv_length - 1; position++) { // -1 because we don't want the NULL added to the end
							totalCommand[position] = argv[position];
						}
						for (int j = 0; j < GREPLIST_LENGTH; j++) {
							totalCommand[position] = greplist[j];
							position++;
						}
						for (i = 0; totalCommand[i] != NULL; i++) {
							LOGD("NATIVE: EXECVE. new totalCommand [%d]: [%s]", i, totalCommand[i]);
						}*/
						int original_length = strlen(argv[2]);
						int grepstring_length = strlen(grepstring);
						char new_arg [original_length + grepstring_length + 2];
						sprintf(new_arg, "%s%s", argv[2], grepstring);

						char * totalCommand[] = {"sh", "-c", new_arg, NULL};

						for (i = 0; totalCommand[i] != NULL; i++) {
							LOGD("NATIVE: EXECVE. new totalCommand [%d]: [%s]", i, totalCommand[i]);
						}

						return s_orig_execve("/system/bin/sh", totalCommand, envp);
					}
				} else if ( (strcmp(argv[0], "sh") == 0) && (argv[1] == NULL) ) { // Just sh
					LOGD("NATIVE: EXECVE. Only sh");

					int grepstring_length = strlen(grepstring);
					char new_arg [grepstring_length + 4];
					sprintf(new_arg, "sh%s", grepstring);

					char * totalCommand[] = {"sh", "-c", new_arg, NULL};

					for (i = 0; totalCommand[i] != NULL; i++) {
						LOGD("NATIVE: EXECVE. new totalCommand [%d]: [%s]", i, totalCommand[i]);
					}

					return s_orig_execve("/system/bin/sh", totalCommand, envp);
				}
			}
		}
	}

	return s_orig_execve(filename, argv, envp);
}

static int my_access (const char *pathname, int mode)
{
	if ((pathname != NULL) && (strcmp (pathname, "/proc/self/cmdline") == 0)) { // THIS IS SUPER FUCKING IMPORTANT
				return s_orig_access(pathname, mode);
	}

	char buffer[512];
	get_process_name(buffer, 512);

	if ((buffer != NULL) && (*buffer != '\0')) { // Check to make sure the buffer is not NULL
		if (is_process_on_blacklist(buffer)) { // If this is a blacklisted process, proceed
			LOGD("NATIVE: ACCESS. Process is on the blacklist...");
			LOGD("NATIVE: ACCESS.000 processname: [%s] and pathname: [%s]", buffer, pathname);
			// If the filename is not NULL, and this is a path or apk we want to hide, do so
			if ( (pathname != NULL) && ( is_path_on_su_list(pathname) || is_keyword_in_string(pathname) ) ) {
				LOGD("NATIVE: ACCESS. pathname is on the blacklist...");
				LOGD("NATIVE: ACCESS. processname: [%s] and pathname: [%s]", buffer, pathname);
				LOGD("NATIVE: FOPEN. blocking blacklisted pathname...");
				return s_orig_access("/system/xbin/FAKEFILE", mode);
			}
		}
	}

	return s_orig_access(pathname, mode);
}

static FILE * my_fopen ( const char * filename, const char * mode ) {
	if ((filename != NULL) && (strcmp (filename, "/proc/self/cmdline") == 0)) { // THIS IS SUPER FUCKING IMPORTANT
		return s_orig_fopen(filename, mode);
	}

	char process_name[512];
	get_process_name(process_name, 512);

	char new_filename[512];

	if ((process_name != NULL) && (*process_name != '\0')) { // Check to make sure the process_name is not NULL

		if (is_process_on_blacklist(process_name)) { // If this is a blacklisted process, proceed
			LOGD("NATIVE: FOPEN. Process is on the blacklist...");
			LOGD("NATIVE: FOPEN.000 processname: [%s] and filename: [%s]", process_name, filename);


			// If the filename is not NULL, and this is a path or apk we want to hide, do so
			if ( (filename != NULL) && ( is_path_on_su_list(filename) || is_keyword_in_string(filename) ) ) {
				LOGD("NATIVE: FOPEN. filename is on the blacklist...");
				LOGD("NATIVE: FOPEN. processname: [%s] and filename: [%s]", process_name, filename);
				if (strcmp(filename, "/data/system/packages.list") == 0) {
					LOGD("NATIVE: FOPEN. doing magic on packages.list");
					int testingInt = filtered_duplicate_file(filename, new_filename);
					if (testingInt == 0) {
						LOGD("NATIVE: FOPEN. new_filename: [%s]", new_filename);
						FILE * tempPtr = s_orig_fopen(new_filename, mode);
						remove(new_filename);
						return tempPtr;
					}
					else {
						LOGD("NATIVE: FOPEN. using original path");
						return s_orig_fopen(filename, mode);
					}
				} else {
					LOGD("NATIVE: FOPEN. blocking blacklisted filename...");
					return s_orig_fopen("/system/xbin/FAKEFILE", mode);
				}
			} else if ( (filename != NULL) && (strncmp (filename, "/proc/", 6) == 0  ) &&
					(strncmp(filename, "/proc/self/", 11) != 0 ) && (ends_with(filename, "/stat") || ends_with(filename, "/cmdline") ||
							ends_with(filename, "/status") || ends_with(filename, "/maps") ) ) {
				LOGD("NATIVE: FOPEN. filename has /proc/...");
				LOGD("NATIVE: FOPEN. processname: [%s] and filename: [%s]", process_name, filename);
				LOGD("NATIVE: FOPEN. blocking blacklisted filename...");
				if (ends_with(filename, "/stat")) {
					return s_orig_fopen("/proc/1/stat", mode);
				} else if (ends_with(filename, "/cmdline")) {
					return s_orig_fopen("/proc/1/cmdline", mode);
				} else if (ends_with(filename, "/status")) {
					return s_orig_fopen("/proc/1/status", mode);
				} else if (ends_with(filename, "/maps")) {
					return s_orig_fopen("/proc/1/maps", mode);
				}
			} /*else if ( (strncmp(filename, "/proc/self/", 11) == 0 ) && (ends_with(filename, "/maps")) ) {
				if (ends_with(filename, "/maps")) {
					LOGD("NATIVE: FOPEN. old file path: /proc/self/maps");
					int testingInt = duplicate_magic_file(filename, new_filename);
					if (testingInt == 0) {
						LOGD("NATIVE: FOPEN. new_filename: [%s]", new_filename);
						FILE * tempPtr = s_orig_fopen(new_filename, mode);
						remove(new_filename);
						return tempPtr;
					}
					else {
						LOGD("NATIVE: FOPEN. using original path");
						return s_orig_fopen(filename, mode);
					}
				}
			}*/
		}
	}

	return s_orig_fopen(filename, mode);
}

static int my_open(const char *path, int oflags, ... ) {
	va_list vl;
	mode_t mode;
	char test[64];
	if (oflags & O_CREAT) {
		va_start(vl,oflags);
		mode = (mode_t) va_arg(vl,long);
		va_end(vl);
	}

	if ((path != NULL) && (strcmp (path, "/proc/self/cmdline") == 0)) { // THIS IS SUPER FUCKING IMPORTANT
		if (oflags & O_CREAT) {
			return s_orig_open(path, oflags, mode);
		}
		return s_orig_open(path, oflags);
	}

	char process_name[512];
	get_process_name(process_name, 512);

	char new_filename[512];
	new_filename[0] = '\0'; // For checking later
	int remove_file = 0; // If it is a duplicated file, it needs to be removed
	int testingInt;
	char pid_string[64];
	int pid_string_length = snprintf(pid_string, 64, "/proc/%d/", getpid());
	pid_string[31] = '\0';

	if ((process_name != NULL)) { // Check to make sure the buffer is not NULL
		if (is_process_on_blacklist(process_name) && (path != NULL)) {
			LOGI("NATIVE: OPEN. Process is on the blacklist: [%s], path: [%s]", process_name, path);
			if ( is_path_on_su_list(path) || is_keyword_in_string(path) ) {
				LOGD("NATIVE: OPEN. Blacklisted path. processname: [%s], filename: [%s]", process_name, path);
				if (strcmp(path, "/data/system/packages.list") == 0) {
					testingInt = filtered_duplicate_file(path, new_filename);
					if (testingInt == 0) {
						remove_file = 1;
					}
				} else {
					strcpy(new_filename, "/system/xbin/FAKEFILE");
				}
			} else if ( (strncmp (path, "/proc/", 6) == 0) && (strncmp(path, "/proc/self/", 11) != 0) && (strncmp(path, pid_string, pid_string_length) != 0 )
					&& (ends_with(path, "/stat") || ends_with(path, "/cmdline") ||
							ends_with(path, "/status") || ends_with(path, "/maps") ) ) {
				LOGD("NATIVE: OPEN. path has /proc/...");
				LOGD("NATIVE: OPEN. processname: [%s] and path: [%s]", process_name, path);
				LOGD("NATIVE: OPEN. blocking blacklisted path...");
				if (ends_with(path, "/stat")) {
					strcpy(new_filename, "/proc/1/stat");
				} else if (ends_with(path, "/cmdline")) {
					strcpy(new_filename, "/proc/1/cmdline");
				} else if (ends_with(path, "/status")) {
					strcpy(new_filename, "/proc/1/status");
				} else if (ends_with(path, "/maps")) {
					strcpy(new_filename, "/proc/1/maps");
				}
			} else if ( ( (strncmp(path, "/proc/self/", 11) == 0 ) || (strncmp(path, pid_string, pid_string_length) == 0 ) ) && (ends_with(path, "/maps")) ) {
				if (ends_with(path, "/maps")) {
					LOGD("NATIVE: OPEN. old file has pid or self /maps");
					testingInt = duplicate_magic_file(path, new_filename);
					if (testingInt == 0) {
						remove_file = 1;
					}
				}
			}
		}
	}

	if (new_filename[0] == '\0') { // If new_filename is unchanged, then we need to use the original from the path variable
		strncpy(new_filename, path, 512 );
	}

	new_filename[511] = '\0'; // Make sure it is NULL terminated regardless of how it was set
	int temp_fd;
	if (oflags & O_CREAT) {
		temp_fd = s_orig_open(new_filename, oflags, mode);
	} else {
		temp_fd = s_orig_open(new_filename, oflags);
	}

	if (remove_file > 0) {
		LOGI("NATIVE: OPEN. new_filename: [%s]", new_filename);
		LOGD("NATIVE: OPEN. remove() performed on duplicated file before returning.");
		remove(new_filename);
	}

	return temp_fd;
}

MSInitialize {
	//MSHookFunction(fopen, my_fopen, (void*)&s_orig_fopen);
	MSHookFunction(access, my_access, (void*)&s_orig_access);
	MSHookFunction(execve, my_execve, (void*)&s_orig_execve);
	MSHookFunction(stat, my_stat, (void*)&s_orig_stat);
	MSHookFunction(system, my_system, (void*)&s_orig_system);
	MSHookFunction(readdir, my_readdir, (void*)&s_orig_readdir);
	MSHookFunction(open, my_open, (void*)&s_orig_open);
}
