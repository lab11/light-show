#ifndef __STREAM_RECEIVER_H__
#define __STREAM_RECEIVER_H__

int streamer_connect (char* query);
json_object* streamer_receive ();

#endif
