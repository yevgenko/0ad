/* Copyright (C) 2019 Wildfire Games.
 * This file is part of 0 A.D.
 *
 * 0 A.D. is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * 0 A.D. is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 0 A.D.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "precompiled.h"

#include <cstdio>

#include "MessagePasserImpl.h"
#include "Messages.h"
#include "Handlers/MessageHandler.h"

#include "lib/timer.h"
#include "lib/rand.h"
#include "lib/posix/posix_filesystem.h"

using namespace AtlasMessage;


MessagePasserImpl::MessagePasserImpl()
: m_Trace(false), m_Semaphore(NULL)
{
	int tries = 0;
	while (tries++ < 16) // some arbitrary cut-off point to avoid infinite loops
	{
		static char name[64];
		sprintf_s(name, ARRAY_SIZE(name), "/wfg-atlas-msgpass-%d-%d",
				(int)rand(1, 1000), (int)(time(0)%1000));
		sem_t* sem = sem_open(name, O_CREAT | O_EXCL, 0700, 0);

		// This cast should not be necessary, but apparently SEM_FAILED is not
		// a value of a pointer type
		if (sem == (sem_t*)SEM_FAILED || !sem)
		{
			int err = errno;
			if (err == EEXIST)
			{
				// Semaphore already exists - try another one
				continue;
			}
			// Otherwise, it's a probably-fatal error
			debug_warn(L"sem_open failed");
			break;
		}
		// Succeeded - use this semaphore
		m_Semaphore = sem;
		m_SemaphoreName = name;
		break;
	}

	if (! m_Semaphore)
	{
		debug_warn(L"Failed to create semaphore for Atlas - giving up");
		// We will probably crash later - maybe we could fall back on sem_init, if this
		// ever fails in practice
	}
}

MessagePasserImpl::~MessagePasserImpl()
{
	if (m_Semaphore)
	{
		// Clean up
		sem_close(m_Semaphore);
		sem_unlink(m_SemaphoreName.c_str());
	}
}

void MessagePasserImpl::Add(IMessage* msg)
{
	ENSURE(msg);
	ENSURE(msg->GetType() == IMessage::Message);

	if (m_Trace)
		debug_printf("%8.3f add message: %s\n", timer_Time(), msg->GetName());

	msgHandlers::const_iterator it = GetMsgHandlers().find(msg->GetName());
	if (it != GetMsgHandlers().end())
	{
		it->second(msg);
	}
	else
	{
		debug_warn(L"Unrecognised message");
		// CLogger might not be initialised, but this error will be sent
		// to the debug output window anyway so people can still see it
		LOGERROR("Unrecognised message (%s)", msg->GetName());
	}
	// Delete the object - we took ownership of it.
	AtlasMessage::ShareableDelete(msg);
}

void MessagePasserImpl::Query(QueryMessage* msg, void(* UNUSED(timeoutCallback) )())
{
	ENSURE(msg);
	ENSURE(msg->GetType() == IMessage::Query);

	if (m_Trace)
		debug_printf("%8.3f add query: %s\n", timer_Time(), msg->GetName());

	msgHandlers::const_iterator it = GetMsgHandlers().find(msg->GetName());
	if (it != GetMsgHandlers().end())
	{
		it->second(msg);
	}
	else
	{
		debug_warn(L"Unrecognised message");
		// CLogger might not be initialised, but this error will be sent
		// to the debug output window anyway so people can still see it
		LOGERROR("Unrecognised message (%s)", msg->GetName());
	}
}

bool MessagePasserImpl::IsEmpty()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_Queue.empty();
}

void MessagePasserImpl::SetTrace(bool t)
{
	m_Trace = t;
}
