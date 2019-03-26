#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include "event.h"
#include "servermanager.h"
#include "event_loop.h"
#include "logger.h"
#include "epoll.h"


void event_handler(event* ev)
{
    if (ev->actives & EPOLLRDHUP)  {

    }
    if (ev->actives & (EPOLLIN | EPOLLPRI))  {
        if (ev->event_read_handler)  {
            ev->event_read_handler(ev->fd, ev, ev->r_arg);
        }
    }
    if (ev->actives & EPOLLOUT)  {
        if (ev->event_write_handler)  {
            ev->event_write_handler(ev->fd, ev, ev->w_arg);
        }
    }
}

event* event_create(int fd, short events, event_callback_pt read_cb,
                    void* r_arg, event_callback_pt write_cb, void* w_arg)
{
    event* ev = (event*)malloc(sizeof(event));
    if (ev == NULL)  {
        debug_ret("file: %s, line: %d", __FILE__, __LINE__);
        return NULL;
    }

    ev->fd = fd;
    ev->events = events;
    ev->actives = 0;
    ev->event_read_handler = read_cb;
    ev->r_arg = r_arg;

    ev->event_write_handler = write_cb;
    ev->w_arg = w_arg;

    return ev;
}

void event_free(event* ev)
{
    event_stop(ev);
	close(ev->fd);
	free(ev);
}

void event_add_io(int epoll_fd, event* ev)
{
    epoller_add(epoll_fd, ev);
    ev->epoll_fd = epoll_fd;
    ev->is_listening = 1;
}

// void event_add_accept(server_manager* manager, event* ev)
// {
//     epoller_add(manager->epoll_fd, ev);
//     ev->epoll_fd = manager->epoll_fd;
//     ev->is_listening = 1;
// }


void event_stop(event *ev)
{
	/* 判断事件ev是否在epoll中,防止重复删除同一事件 */
	if (ev->is_listening == 0)
		return;

    epoller_del(ev->epoll_fd, ev);

	ev->is_listening = 0;
}