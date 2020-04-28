/* single.c: Single User HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <string.h>

#include <unistd.h>

/**
 * Handle one HTTP request at a time.
 *
 * @param   sfd         Server socket file descriptor.
 * @return  Exit status of server (EXIT_SUCCESS).
 **/
int single_server(int sfd) {
    /* Accept and handle HTTP request */
    while (true) {
    	/* Accept request */
        Request *request = accept_request(sfd);
        if (!request) {
            log("Unable to accept request: %s", strerror(errno));
            continue;
        }

	/* Handle request */
        Status s;
        if ((s = handle_request(request)) != HTTP_STATUS_OK) {
            debug("handle request failed: %s", http_status_string(s));
        }
	
        /* Free request */
        free_request(request);
    }

    /* Close server socket */
    close(sfd);
    return EXIT_SUCCESS;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
