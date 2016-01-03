/**
 * @file dll/create.c
 *
 * @copyright 2015 Bill Zissimopoulos
 */

#include <dll/library.h>

static NTSTATUS FspFileSystemOpCreate_FileCreate(FSP_FILE_SYSTEM *FileSystem,
    FSP_FSCTL_TRANSACT_REQ *Request)
{
    if (0 == FileSystem->Interface->FileCreate)
        return FspFileSystemSendResponseWithStatus(FileSystem, Request, STATUS_NOT_IMPLEMENTED);

    NTSTATUS Result;
    DWORD GrantedAccess;
    PVOID File;
    FSP_FSCTL_TRANSACT_RSP Response;

    Result = FspAccessCheck(FileSystem, Request, TRUE,
        (Request->Req.Create.CreateOptions & FILE_DIRECTORY_FILE) ?
            FILE_ADD_SUBDIRECTORY : FILE_ADD_FILE,
        &GrantedAccess);
    if (!NT_SUCCESS(Result))
        return FspFileSystemSendResponseWithStatus(FileSystem, Request, Result);

    Result = FileSystem->Interface->FileCreate(FileSystem, Request, &File);
    if (!NT_SUCCESS(Result))
        return FspFileSystemSendResponseWithStatus(FileSystem, Request, Result);

    /* !!!: set share access */

    memset(&Response, 0, sizeof Response);
    Response.Size = sizeof Response;
    Response.Kind = Request->Kind;
    Response.Hint = Request->Hint;
    Response.IoStatus.Status = STATUS_SUCCESS;
    Response.IoStatus.Information = FILE_CREATED;
    Response.Rsp.Create.Opened.UserContext = (UINT_PTR)File;
    Response.Rsp.Create.Opened.GrantedAccess = GrantedAccess;
    return FspFileSystemSendResponse(FileSystem, &Response);
}

static NTSTATUS FspFileSystemOpCreate_FileOpen(FSP_FILE_SYSTEM *FileSystem,
    FSP_FSCTL_TRANSACT_REQ *Request)
{
    return STATUS_NOT_IMPLEMENTED;
#if 0
    NTSTATUS Result;
    DWORD GrantedAccess;
    PVOID File;
    FSP_FSCTL_TRANSACT_RSP Response;

    Result = FspAccessCheck(FileSystem, Request, FALSE, &GrantedAccess);
    if (!NT_SUCCESS(Result))
        return FspSendResponseWithStatus(FileSystem, Request, Result);

    Result = FileSystem->FileOpen(FileSystem, Request, &File);
    if (!NT_SUCCESS(Result))
        return FspSendResponseWithStatus(FileSystem, Request, Result);

    Result = FspShareCheck(FileSystem, Request, GrantedAccess, File);
    if (!NT_SUCCESS(Result))
    {
        FileSystem->FileCleanupClose(FileSystem, Request, File);
        return FspSendResponseWithStatus(FileSystem, Request, Result);
    }

    memset(&Response, 0, sizeof Response);
    Response.Size = sizeof Response;
    Response.Kind = Request->Kind;
    Response.Hint = Request->Hint;
    Response.IoStatus.Status = STATUS_SUCCESS;
    Response.IoStatus.Information = FILE_CREATED;
    Response.Rsp.Create.Opened.UserContext = (UINT_PTR)File;
    Response.Rsp.Create.Opened.GrantedAccess = GrantedAccess;
    return FspSendResponse(FileSystem, &Response);
#endif
}

static NTSTATUS FspFileSystemOpCreate_FileOpenIf(FSP_FILE_SYSTEM *FileSystem,
    FSP_FSCTL_TRANSACT_REQ *Request)
{
    return STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS FspFileSystemOpCreate_FileOverwrite(FSP_FILE_SYSTEM *FileSystem,
    FSP_FSCTL_TRANSACT_REQ *Request, BOOLEAN Supersede)
{
    return STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS FspFileSystemOpCreate_FileOverwriteIf(FSP_FILE_SYSTEM *FileSystem,
    FSP_FSCTL_TRANSACT_REQ *Request)
{
    return STATUS_NOT_IMPLEMENTED;
}

FSP_API NTSTATUS FspFileSystemOpCreate(FSP_FILE_SYSTEM *FileSystem,
    FSP_FSCTL_TRANSACT_REQ *Request)
{
    switch ((Request->Req.Create.CreateOptions >> 24) & 0xff)
    {
    case FILE_CREATE:
        return FspFileSystemOpCreate_FileCreate(FileSystem, Request);
    case FILE_OPEN:
        return FspFileSystemOpCreate_FileOpen(FileSystem, Request);
    case FILE_OPEN_IF:
        return FspFileSystemOpCreate_FileOpenIf(FileSystem, Request);
    case FILE_OVERWRITE:
        return FspFileSystemOpCreate_FileOverwrite(FileSystem, Request, FALSE);
    case FILE_SUPERSEDE:
        return FspFileSystemOpCreate_FileOverwrite(FileSystem, Request, TRUE);
    case FILE_OVERWRITE_IF:
        return FspFileSystemOpCreate_FileOverwriteIf(FileSystem, Request);
    default:
        return FspFileSystemSendResponseWithStatus(FileSystem, Request, STATUS_INVALID_PARAMETER);
    }
}