#ifndef ERROR_H
#define ERROR_H

// Helper function used in setting formatted error message in the shared global error buffer
void setError(const char* fmt, ...);
// Retrieves currently set message in the global error buffer
const char* getLastError(void);

#endif