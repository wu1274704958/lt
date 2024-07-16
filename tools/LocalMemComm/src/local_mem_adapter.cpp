#include <local_mem_adapter.h>

#if WIN32


bool eqd::win_local_mem_adapter::init(const std::string& mem_id, uint32_t size)
{
    mem_handle = ::CreateFileMapping(INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        size,
        mem_id.c_str());
    if (NULL == mem_handle)
    {
        _last_error = "Create file mapping failed " + ::GetLastError();
        return false;
    }
    ptr = (uint8_t*)::MapViewOfFile(mem_handle,
        FILE_MAP_ALL_ACCESS,
        0,
        0,      //memory start address  
        size);     //all memory space  
    if (nullptr == ptr)
    {
        _last_error = "Map memory failed " + ::GetLastError();
        release();
        return false;
    }
    if (!create_event(mem_id))
    {
        release();
        return false;
    }

	return true;
}

bool eqd::win_local_mem_adapter::create_event(const std::string& mem_id)
{
    std::string evt_name = "GlobalEvt_";
    evt_name += mem_id;
    event_handle = OpenEvent(EVENT_ALL_ACCESS, FALSE, evt_name.c_str());

    if (event_handle == NULL)
    {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_FILE_NOT_FOUND) {
            event_handle = CreateEvent(
                NULL,  
                TRUE,  
                TRUE,  
                evt_name.c_str() 
            );

            if (event_handle == NULL) {
                _last_error = "Create event failed " + ::GetLastError();
                return false;
            }
        }
        else {
            _last_error = "Open event failed " + ::GetLastError();
            return false;
        }
    }
    return true;
}

void eqd::win_local_mem_adapter::release()
{
    if (event_handle != NULL)
    {
        CloseHandle(event_handle);
        event_handle = NULL;
    }
    if (ptr != nullptr)
    {
        UnmapViewOfFile(ptr);
        ptr = nullptr;
    }
    if (mem_handle != NULL)
    {
        CloseHandle(mem_handle);
        mem_handle = NULL;
    }
}

const std::string& eqd::win_local_mem_adapter::last_error()
{
	return _last_error;
}

uint8_t* eqd::win_local_mem_adapter::get_mem()
{
	return ptr;
}

bool eqd::win_local_mem_adapter::is_mem_idle()
{
    DWORD waitResult = WaitForSingleObject(event_handle, 0);
    return waitResult == WAIT_OBJECT_0;
}
bool eqd::win_local_mem_adapter::try_lock_mem()
{
    if (WaitForSingleObject(event_handle, 0) == WAIT_OBJECT_0)
    {
        return ResetEvent(event_handle);
    }
    return false;
}
bool eqd::win_local_mem_adapter::try_wait_lock_mem()
{
    if (WaitForSingleObject(event_handle, INFINITE) == WAIT_OBJECT_0)
    {
        return ResetEvent(event_handle);
    }
    return false;
}
void eqd::win_local_mem_adapter::unlock_mem()
{
    SetEvent(event_handle);
}

#endif // WIN32

