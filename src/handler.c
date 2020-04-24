/* handler.c: HTTP Request Handlers */

#include "spidey.h"

#include <errno.h>
#include <limits.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/* Internal Declarations */
Status handle_browse_request(Request *request);
Status handle_file_request(Request *request);
Status handle_cgi_request(Request *request);
Status handle_error(Request *request, Status status);

/**
 * Handle HTTP Request.
 *
 * @param   r           HTTP Request structure
 * @return  Status of the HTTP request.
 *
 * This parses a request, determines the request path, determines the request
 * type, and then dispatches to the appropriate handler type.
 *
 * On error, handle_error should be used with an appropriate HTTP status code.
 **/
Status  handle_request(Request *r) {
    Status result;
    struct stat s;
    bool is_dir;
    bool is_exec;

    if(parse_request(r) < 0){
        debug("parse_request");
        return NULL;
    }

    /* Determine request path */
    r->path = determine_request_path(r->uri);
    debug("HTTP REQUEST PATH: %s", r->path);

    /* Dispatch to appropriate request handler type based on file type */
    is_dir = (stat(r->path, &s) == 0 && !S_ISDIR(s.st_mode));
    
    if (is_dir)
        result = handle_browse_request(r);
    else {
        is_exec = access(r->path, X_OK);
        if (is_exec)
            result = handle_cgi_request(r);
        else 
            result = handle_file_request(r);
    }
    // figure out if there is an error
    if (result != HTTP_STATUS_OK) 
        handle_error(r);

    log("HTTP REQUEST STATUS: %s", http_status_string(result));

    return result;
}

/**
 * Handle browse request.
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP browse request.
 *
 * This lists the contents of a directory in HTML.
 *
 * If the path cannot be opened or scanned as a directory, then handle error
 * with HTTP_STATUS_NOT_FOUND.
 **/
Status  handle_browse_request(Request *r) {
    struct dirent **entries;
    int n;

    /* Open a directory for reading or scanning */
    int n = scandir(".", entries, 0, alphasort);
    if (n < 0) {
        debug("scandir failed: %s\n", strerror(errno));
        return HTTP_STATUS_NOT_FOUND;
    }

    /* Write HTTP Header with OK Status and text/html Content-Type */
    fprintf(r->stream, "HTTP/1.0 200 OK\r\n");
    fprintf(r->stream, "Content-Type: text/html\r\n");
    fprintf(r->stream, "\r\n");


    /* For each entry in directory, emit HTML list item */
    // this prints text, not links
    fprintf(r->stream, "<ul>\n");
    for(int i = 0; i < n; i++){
        fprintf(r->stream, "<li>%s</li>\n", entries[i]->d_name);
    }
    fprintf(r->stream, "</ul>\n");
    /* Return OK */
    return HTTP_STATUS_OK;
}

/**
 * Handle file request.
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP file request.
 *
 * This opens and streams the contents of the specified file to the socket.
 *
 * If the path cannot be opened for reading, then handle error with
 * HTTP_STATUS_NOT_FOUND.
 **/
Status  handle_file_request(Request *r) {
    FILE *fs;
    char buffer[BUFSIZ];
    char *mimetype = NULL;
    size_t nread;

    /* Open file for reading */
    fs = fopen(r->path, "r");
    if(!fs) {
        debug("fopen failed: %s\n", strerror(errno));
        return HTTP_STATUS_NOT_FOUND;
    }

    /* Determine mimetype */
    mimetype = determine_mimetype(r->path);

    /* Write HTTP Headers with OK status and determined Content-Type */
    fprintf(r->stream, "HTTP/1.0 200 OK\r\n");
    fprintf(r->stream, "Content-Type: %s\r\n", mimetype);
    fprintf(r->stream, "\r\n");
    

    /* Read from file and write to socket in chunks */
    while ((nread = fread(buffer, 1, BUFSIZ, fs)) > 0) {
        fwrite(buffer, 1, nread, r->stream);
    }

    fclose(fs);

    /* Close file, deallocate mimetype, return OK */
    return HTTP_STATUS_OK;

fail:
    /* Close file, free mimetype, return INTERNAL_SERVER_ERROR */
    return HTTP_STATUS_INTERNAL_SERVER_ERROR;
}

/**
 * Handle CGI request
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP file request.
 *
 * This popens and streams the results of the specified executables to the
 * socket.
 *
 * If the path cannot be popened, then handle error with
 * HTTP_STATUS_INTERNAL_SERVER_ERROR.
 **/
Status  handle_cgi_request(Request *r) {
    FILE *pfs;
    char buffer[BUFSIZ];

    /* Export CGI environment variables from request:
     * http://en.wikipedia.org/wiki/Common_Gateway_Interface */
    setenv("QUERY_STRING", r->query);

    /* Export CGI environment variables from request headers */
    for(Header *h=r->headers; h; h=h->next){
        setenv(h->name, h->data);
    }
    /* POpen CGI Script */
    pfs = popen(r->path, "r");
    if(!pfs){
        debug("popen: %s", strerror(errno));
        return HTTP_STATUS_INTERNAL_SERVER_ERROR;
    }
    /* Copy data from popen to socket */
    size_t nread = fread(buffer, 1, BUFSIZ, pfs);
    while (nread > 0){
        fwrite(buffer, 1, nread, pfs);
        nread = fread(buffer, 1, BUFSIZ, pfs);
    }

    pclose(pfs);
    /* Close popen, return OK */
    return HTTP_STATUS_OK;
}

/**
 * Handle displaying error page
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP error request.
 *
 * This writes an HTTP status error code and then generates an HTML message to
 * notify the user of the error.
 **/
Status  handle_error(Request *r, Status status) {
    const char *status_string = http_status_string(status);

    /* Write HTTP Header */

    /* Write HTML Description of Error*/

    /* Return specified status */
    return status;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
