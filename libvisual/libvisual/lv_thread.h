#ifndef _LV_THREAD_H
#define _LV_THREAD_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>

#if defined(VISUAL_OS_WIN32)
#include <windows.h>
#endif

#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
#include <pthread.h>
#elif defined(VISUAL_THREAD_MODEL_GTHREAD2) /* !VISUAL_THREAD_MODEL_POSIX */
#include <glib/gthread.h>
#else /* !VISUAL_THREAD_MODEL_GTHREAD2 */

#endif
#endif /* VISUAL_HAVE_THREADS */

/**
 * @defgroup VisThread VisThread
 * @{
 */

VISUAL_BEGIN_DECLS

typedef struct _VisThread VisThread;
typedef struct _VisMutex VisMutex;

/**
 * The function defination for a function that forms the base of a new VisThread when
 * visual_thread_create is used.
 *
 * @see visual_thread_create
 *
 * @arg data Pointer that can contains the private data from the visual_thread_create function.
 *
 * @return Pointer to the data when a thread is joined.
 */
typedef void *(*VisThreadFunc)(void *data);

/**
 * The VisThread data structure and the VisThread subsystem is a wrapper system for native
 * threading implementations.
 */
struct _VisThread {
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	pthread_t thread;		/**< Private used for the pthread implementation. */
#elif defined(VISUAL_THREAD_MODEL_WIN32) /* !VISUAL_THREAD_MODEL_POSIX */
	HANDLE thread;
	DWORD threadId;
#elif defined(VISUAL_THREAD_MODEL_GTHREAD) /* !VISUAL_THREAD_MODEL_WIN32 */
	GThread *thread;
#endif
#endif /* VISUAL_HAVE_THREADS */
};

/**
 * The VisMutex data structure and the VisMutex subsystem is a wrapper system for native
 * thread locking implementations.
 */
struct _VisMutex {
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	pthread_mutex_t mutex;		/**< Private used for the pthreads implementation. */
#elif defined(VISUAL_THREAD_MODEL_WIN32) /* !VISUAL_THREAD_MODEL_POSIX */

#elif defined(VISUAL_THREAD_MODEL_GTHREAD) /* !VISUAL_THREAD_MODEL_WIN32 */
	GMutex *mutex;

	GStaticMutex static_mutex;
	int static_mutex_used;
#endif
#endif /* VISUAL_HAVE_THREADS */
};


/**
 * Initializes the VisThread subsystem. This function needs to be
 * called before VisThread can be used. Also this function is called
 * from within visual_init().
 *
 * @return TRUE if initialized, FALSE if not initialized.
 */
int visual_thread_initialize (void);

/**
 * Request if VisThread is initialized or not. This function should
 * not be confused with visual_thread_is_supported().
 *
 * @return TRUE if initialized, FALSE if not initialized.
 */
int visual_thread_is_initialized (void);


/**
 * Enable or disable threading support. This can be used to disallow
 * threads, which might be needed in some environments.
 *
 * @see visual_thread_is_enabled
 *
 * @param enabled TRUE to enable threads, FALSE to disable threads.
 */
void visual_thread_enable (int enabled);

/**
 * Request if threads are enabled or not. This function should not be
 * confused with visual_thread_is_supported().
 *
 * @return TRUE if enabled, FALSE if disabled.
 */
int visual_thread_is_enabled (void);


/**
 * Is used to check if threading is supported. When threading is used
 * this should always be checked, it's possible to disable threads
 * from within the code, so no \#ifdefs should be used.
 *
 * @return TRUE if threading is supported or FALSE if not.
 */
int visual_thread_is_supported (void);

/**
 * Creates a new VisThread that is used in threading.
 *
 * @param func The threading function.
 * @param data The private data that is send to the threading function.
 * @param joinable Flag that contains whatever the thread can be joined or not.
 *
 * @return A newly allocated VisThread, or NULL on failure.
 */
VisThread *visual_thread_create (VisThreadFunc func, void *data, int joinable);

/**
 * After a VisThread is not needed anylonger it needs to be freed
 * using this function.
 *
 * @param thread The VisThread that needs to be freed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_THREAD_NULL or
 *	error values returned by visual_mem_free on failure.
 */
int visual_thread_free (VisThread *thread);

/**
 * Joins a VisThread with another.
 *
 * @param thread The VisThread that is about to be joined.
 *
 * @return Possible result that was passed to visual_thread_exit as retval or
 *	NULL.
 */
void *visual_thread_join (VisThread *thread);

/**
 * Exits a VisThread, this will terminate the thread.
 *
 * @param retval The return value that is catched by the visual_thread_join function.
 */
void visual_thread_exit (void *retval);

/**
 * Yield the current VisThread so another gets time.
 */
void visual_thread_yield (void);

/**
 * Creates a new VisMutex that is used to do thread locking so data
 * can be synchronized.
 *
 * @return A newly allocated VisMutex that can be used with the visual_mutex_lock and
 *	visual_mutex_unlock functions or NULL on failure.
 */
VisMutex *visual_mutex_new (void);

/**
 * A VisMutex is allocated to have more flexibility with the actual
 * thread backend. Thus they need to be freed as well.
 *
 * @param mutex Pointer to the VisMutex that needs to be freed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MUTEX_NULL on failure.
 */
int visual_mutex_free (VisMutex *mutex);

/**
 * A VisMutex that has not been allocated using visual_mutex_new ()
 * can be initialized using this function. You can use non allocated
 * VisMutex variables in this context by giving a reference to them.
 *
 * @param mutex Pointer to the VisMutex which needs to be initialized.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MUTEX_NULL,  -VISUAL_ERROR_THREAD_NOT_INTIALIZED,
 * 	-VISUAL_ERROR_THREAD_NOT_SUPPORTED or -VISUAL_ERROR_THREAD_NOT_ENABLED on failure.
 */
int visual_mutex_init (VisMutex *mutex);


/**
 * Locks a VisMutex, with the VisMutex locks checked right, only one
 * thread can access the area at once.  This will block if the thread
 * is already locked.
 *
 * @param mutex Pointer to the VisMutex to register the lock.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MUTEX_NULL, -VISUAL_ERROR_MUTEX_LOCK_FAILURE,
 *	-VISUAL_ERROR_THREAD_NOT_INITIALIZED, -VISUAL_ERROR_THREAD_NOT_SUPPORTED,
 *	-VISUAL_ERROR_THREAD_NOT_ENABLED on failure.
 */
int visual_mutex_lock (VisMutex *mutex);

/**
 * Tries to lock a VisMutex, however instead of visual_mutex_lock it
 * does not block on failure. but returns instead with
 * -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE as error value.
 *
 * @param mutex Pointer to the VisMutex that needs to be locked.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MUTEX_NULL, -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE,
 *	-VISUAL_ERROR_THREAD_NOT_INITIALIZED, -VISUAL_ERROR_THREAD_NOT_SUPPORTED or
 *	-VISUAL_ERROR_THREAD_NOT_ENABLED on failure.
 */
int visual_mutex_trylock (VisMutex *mutex);

/**
 * Unlocks a VisMutex so other threads that use the same lock can now
 * enter the critical area.
 *
 * @param mutex Pointer to the VisMutex that is unlocked.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MUTEX_NULL, -VISUAL_ERROR_MUTEX_UNLOCK_FAILURE.
 *	-VISUAL_ERROR_THREAD_NOT_INITIALIZED, -VISUAL_ERROR_THREAD_NOT_SUPPORTED or
 *	-VISUAL_ERROR_THREAD_NOT_ENABLED on failure.
 */
int visual_mutex_unlock (VisMutex *mutex);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_THREAD_H */
