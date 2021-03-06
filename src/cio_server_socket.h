/*
 * The MIT License (MIT)
 *
 * Copyright (c) <2017> <Stephan Gatzka>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CIO_SERVER_SOCKET_H
#define CIO_SERVER_SOCKET_H

#include <stdbool.h>
#include <stdint.h>

#include "cio_error_code.h"
#include "cio_eventloop.h"
#include "cio_socket.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief This file contains the interface of a server socket.
 *
 * A server socket can be @ref cio_server_socket_init "initialized",
 * @ref cio_server_socket_accept "accept connections" and can be
 * @ref cio_server_socket_close "closed".
 */

struct cio_server_socket;

/**
 * @brief The type of a function that is called when
 * @ref cio_server_socket_accept "accept task" succeeds or fails.
 *
 * @param handler_context The context the functions works on.
 * @param err If err != ::cio_success, the read failed.
 * @param buf A pointer to the begin of the buffer where the data was read in.
 * @param bytes_transferred The number of bytes transferred into @p buf.
 */
typedef void (*cio_accept_handler)(struct cio_server_socket *ss, void *handler_context, enum cio_error err, struct cio_socket *socket);

/**
 * @brief The type of close hook function.
 *
 * @param ss The cio_linux_server_socket the close hook was called on.
 */
typedef void (*cio_server_socket_close_hook)(struct cio_server_socket *ss);

/**
 * @brief The cio_server_socket struct describes a server socket.
 */
struct cio_server_socket {
	/**
	 * @brief The context pointer which is passed to the functions
	 * specified below.
	 */
	void *context;

	/**
	 * @anchor cio_server_socket_init
	 * @brief Initializes a cio_server_socket.
	 *
	 * If this function succeeds, the server socket is bound an listens on the
	 * port specified. If later on something goes wrong, it's the responsibility
	 * of the user to call @ref cio_server_socket_close "close" on the server socket.
	 *
	 * @param context The cio_server_socket::context.
	 * @param backlog The minimal length of the listen queue.
	 * If @a bind_address is @p NULL, cio_server_socket will bind to any interface.
	 *
	 * @return ::cio_success for success.
	 */
	enum cio_error (*init)(void *context, unsigned int backlog);

	/**
	 * @anchor cio_server_socket_accept
	 * @brief Accepts an incoming socket connection.
	 *
	 * @param context The cio_server_socket::context.
	 * @param handler The function to be called if the accept failes or succeeds.
	 * @param handler_context The context passed the the @a handler function.
	 *
	 * @return ::cio_success for success.
	 */
	enum cio_error (*accept)(void *context, cio_accept_handler handler, void *handler_context);

	/**
	 * @anchor cio_server_socket_close
	 * @brief Closes the cio_server_socket.
	 *
	 * @param context The cio_server_socket::context.
	 */
	void (*close)(void *context);

	/**
	 * @anchor cio_server_socket_bind
	 * @brief Binds the cio_server_socket to a specific address
	 *
	 * @param context The cio_server_socket::context.
	 * @param bind_address The IP address a cio_server_socket shall bound to.
	 * @param port The TCP port the cio_server_socket shall listen on.
	 *
	 * @return ::cio_success for success.
	 */
	enum cio_error (*bind)(void *context, const char *bind_address, uint16_t port);

	/**
	 * @anchor cio_server_socket_set_reuse_address
	 * @brief Sets the SO_REUSEADDR socket option.
	 *
	 * @param context The cio_server_socket::context.
	 * @param on Whether the socket option should be enabled or disabled.
	 *
	 * @return ::cio_success for success.
	 */
	enum cio_error (*set_reuse_address)(void *context, bool on);

	/**
	 * @privatesection
	 */
	struct cio_eventloop *loop;
	int backlog;
	cio_server_socket_close_hook close_hook;
	struct cio_event_notifier ev;
	cio_accept_handler handler;
	void *handler_context;
};

/**
 * @brief Initializes a cio_server_socket.
 *
 * @param ss The cio_server socket that should be initialized.
 * @param loop The event loop the server socket shall operate on.
 * @param close A close hook function. If this parameter is non @p NULL,
 * the function will be called directly after
 * @ref cio_server_socket_close "closing" the cio_server_socket.
 * It is guaranteed the the cio library will not access any memory of
 * cio_server_socket that is passed to the close hook. Therefore
 * the hook could be used to free the memory of the server socket.
 * @return The cio_server_socket which shall be used after initializing.
 */
void cio_server_socket_init(struct cio_server_socket *ss,
                            struct cio_eventloop *loop,
                            cio_server_socket_close_hook close);

#ifdef __cplusplus
}
#endif

#endif
