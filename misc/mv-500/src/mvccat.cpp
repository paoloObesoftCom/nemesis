#include <stdio.h>
#include "defs.h"
#include "conf.h"
#include "util.h"
#include "inputdefreader.h"

struct Inputs inputs;
struct Conf conf;
struct ViewConf views[NO_VIEWS];

// Changing this, need to change version in util.cpp

static eString dirs[7] = {
        "/var/mnt/usb/mv",
        "/mnt/usb/mv",
        "/var/mnt/cf/mv",
        "/mnt/cf/mv",
        "/hdd/mv",
        "/var/tuxbox/config/mv",
        "/var/tmp/mv"
};

char * getStr( unsigned int strNo ) {
	return "erreur";
}

void dmsg( char * msg, char *msg2 )
{
	fprintf(stderr,"Erreur %s. %s.\n",msg,msg2);
}

int fileSize( eString filePath ) {
	struct stat stats;	
	if ( stat( filePath.c_str(), &stats ) == -1 ) {
		return -1;
	}
	else {
		return stats.st_size;
	}
}

int pathExists( eString path )
{
	 return ( fileSize( path ) != -1 );
}

eString & indexToDir( int index )
{
        return dirs[index];
}

int readConfig( void );
unsigned int calcConfigChecksum( void );

int main( const char *argv, int argc )
{
 InputDefReader defs;
	if ( readConfig() == configErrorNone ) {
	    fprintf( stdout, "STORAGEDIR=%s\n", indexToDir( inputs.storageDir ).c_str() );
	    for ( int dno = 0; dno < MAX_NO_INPUTS; dno++ ) {
		if ( ! inputs.confs[dno].enabledFlag )
			continue;
		struct inputDef *defP = defs.getDefP( inputs.confs[dno].name );
		if ( defP == NULL ) 
			continue;

		fprintf( stdout, "R%d=%s\n",dno,defP->remoteDir);
		fprintf( stdout, "L%d=%s\n",dno,defP->localDir);
		fprintf( stdout, "P%d=%s\n",dno,defP->prefix);
		fprintf( stdout, "A%d=%s\n",dno,defP->affix);
		fprintf( stdout, "D%d=%d\n",dno,defP->days);
		fprintf( stdout, "T%d=%c\n",dno,defP->type);
	    }
		return 0;
	}
	else {
	    fprintf( stdout, "STORAGEDIR=%s\n", indexToDir( inputs.storageDir ).c_str() );
		return 1;
	}
}

int readConfig( void )
{
        eString configPath = eString( CONFIG_DIR ) + "/" + eString( CONFIG_FILENAME );

        FILE *fp = fopen( configPath.c_str(), "rb" );
        if ( ! fp ) {
		fprintf( stderr, "%s: not found\n", configPath.c_str() );
		return configErrorNotFound;
        }
        else {
                size_t read = fread( &inputs, sizeof( struct Inputs ),
 1, fp );
                read += fread( &conf, sizeof( struct Conf ), 1, fp );
                read += fread( views, sizeof( struct ViewConf ), NO_VIEWS, fp );
                fclose( fp );

                if ( read != ( 2 + NO_VIEWS ) ) {
			fprintf( stderr, "%s: corrupt (size)\n", configPath.c_str() );
                        return configErrorBadFile;
		}

                unsigned int tempChecksum = conf.checksum;
                if ( tempChecksum != calcConfigChecksum() ) {
			fprintf( stderr, "%s: corrupt (checksum)\n", configPath.c_str() );
                        return configErrorBadFile;
		}
        }

        return configErrorNone;
}

unsigned int calcConfigChecksum( void )
{
        // Don't allow checksum itself to affect the result
        // of the calculation
        conf.checksum = 0;

        unsigned int csum = 0;

        // We want to calc checksum over all of conf, view
        // confs and inputs
        char * ptrs[3] = {
                (char *) &conf, (char *) views, (char *) &inputs
        };
        unsigned int sizes[3] = {
                sizeof( struct Conf ),
                sizeof( struct ViewConf ) * NO_VIEWS,
                sizeof( struct Inputs )
        };

        for ( unsigned int ptrNo= 0; ptrNo < 3; ptrNo++ ) {
                char *d = ptrs[ptrNo];
                unsigned int structSize = sizes[ptrNo];

                for ( unsigned int i = 0; i < structSize; i++ ) {
                        csum += (unsigned int) (*((unsigned char *) d)
);
                        d++;
                }
        }

        return csum;
}



