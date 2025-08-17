#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/time.h>
 
#include "data_type.h"
#include "cube.h"
#include "cube_define.h"
#include "cube_record.h"
#include "user_define.h"
#include "file_struct.h"
#include "mac_label.h"
// add para lib_include
int file_acl_init(void * sub_proc, void * para)
{
	int ret;
	// add yorself's module init func here
	return 0;
}
int file_acl_start(void * sub_proc, void * para)
{
	int ret;
	void * recv_msg;
	int type;
	int subtype;
	// add yorself's module exec func here
	while(1)
	{
		usleep(time_val.tv_usec);
		ret=ex_module_recvmsg(sub_proc,&recv_msg);
		if(ret<0)
			continue;
		if(recv_msg==NULL)
			continue;
		type=message_get_type(recv_msg);
		subtype=message_get_subtype(recv_msg);
		if(!memdb_find_recordtype(type,subtype))
		{
			printf("message format (%d %d) is not registered!\n",
			message_get_type(recv_msg),message_get_subtype(recv_msg));
			continue;
		}
		if((type==TYPE(FILE_TRANS))&&(subtype==SUBTYPE(FILE_TRANS,REQUEST)))
		{
			ret=proc_get_file_acl(sub_proc,recv_msg);
		}
	}
	return 0;
}

int read_acl(RECORD(MAC_LABEL,SUBJECT) * sub_label,RECORD(MAC_LABEL,OBJECT) * obj_label)
{
//  add biba module access control code here
//  return 1 means access is permitted

	return 0;	
}


int proc_get_file_acl(void * sub_proc,void * recv_msg)
{
	int ret;
	RECORD(FILE_TRANS,REQUEST) * file_req;
	RECORD(FILE_TRANS,FILE_NOTICE) * err_return;
	RECORD(MAC_LABEL,SUBJECT) * user_label;
	RECORD(MAC_LABEL,OBJECT) * file_label;
	MSG_EXPAND * msg_expand;
	DB_RECORD * db_record;
	void * new_msg;
	
	// get file_req struct from message
	ret=message_get_record(recv_msg,&file_req,0);
	if(ret<0)
		return ret;

	// get user label from message's expand record
	ret=message_remove_expand(recv_msg,TYPE_PAIR(MAC_LABEL,SUBJECT),&msg_expand);
	if(ret<0)
		return ret;
	if(msg_expand==NULL)
	{
		print_cubeerr("can't find user attached!\n");	
		return -EINVAL;
	}
	user_label=msg_expand->expand;

	// lookup file label from memdb by filename
	db_record=memdb_find_first(TYPE_PAIR(MAC_LABEL,OBJECT),"file_name",file_req->filename);
	if(db_record==NULL)
	{
		// can't find file's object label, refuse access
		err_return=Talloc0(sizeof(*err_return));
		if(err_return == NULL)
			return -EINVAL;
		err_return->result=-1;
		err_return->filename = dup_str(file_req->filename,0);
		new_msg = message_create(TYPE_PAIR(FILE_TRANS,FILE_NOTICE),recv_msg);
		if(new_msg==NULL)
			return -EINVAL;
		message_add_record(new_msg,err_return);
		ret=ex_module_sendmsg(sub_proc,new_msg);		
	}
	else
	{
        file_label=db_record->record;
		if(read_acl(user_label,file_label))
		{
			ret=ex_module_sendmsg(sub_proc,recv_msg);
		}
		else
		{
			// can't find file's object label, refuse access
			err_return=Talloc0(sizeof(*err_return));
			if(err_return == NULL)
				return -EINVAL;
			err_return->result=-2;
		    err_return->filename = dup_str(file_req->filename,0);
		    new_msg = message_create(TYPE_PAIR(FILE_TRANS,FILE_NOTICE),recv_msg);
			if(new_msg==NULL)
				return -EINVAL;
			message_add_record(new_msg,err_return);
			ret=ex_module_sendmsg(sub_proc,new_msg);		
		}	
	}	
	return ret;
}
