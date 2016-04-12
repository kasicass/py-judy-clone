#ifndef PYJUDY_H
#define PYJUDY_H

#include <Python.h>
#include <Judy.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  PyObject_HEAD
  Pvoid_t array;
} PyJudyL;

typedef struct {
  PyObject_HEAD
  Pvoid_t array;
} PyJudy1;

typedef struct {
  PyObject_HEAD
  Pvoid_t array;
  /* There's no JudySLCount, which I need to make things like .keys()   */
  /* easier to implement and to support __len__.  Instead, I'll track   */
  /* all successful inserts and deletes.*/
  Word_t count;  /* Change this if you modify the array in C code! */

  /* Track the largest inserted string size */
  /* Needed for Last() as an optimization to fill the index buffer */
  /* with max_seen number of 0xFF characters. */
  int max_seen;
     
} PyJudySL;


#ifdef __cplusplus
}
#endif

#endif  /* !PYJUDY_H */
