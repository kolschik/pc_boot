/*
 * Copyright 2013, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may not
 * reproduce, modify or distribute this software except in compliance with the
 * License. You may obtain a copy of the License at:
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis, WITHOUT
 * WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as contributors under
 * the License or as licensors under other terms.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 */

#ifndef _AT91SAMA5D3_DMA_DEVCTL_H_
#define _AT91SAMA5D3_DMA_DEVCTL_H_

#include <devctl.h>


/* Constants
 * ---------------------------------------------------------- */
#define DMA_DRIVER_NAME		"/dev/dma"

#define ENOTOK					-1
#define	INVALID_DESCRIPTOR		-1

#define DMA_CALLBACK_PULSE_CODE    (_PULSE_CODE_MINAVAIL)


/* All IOCTL API's should be bi-directional (e.g. req/rsp) so we can initialise our enum
 * to the start of the TOFROM devctl cmd number space.
 */
typedef enum
{
	DMA_DEVCTL_ALLOCATE	= _POSIX_DEVDIR_TOFROM,
	DMA_DEVCTL_FREE
} t_dma_devctl_request_type;


/* Data to be passed through during DEVCTLs
 * ---------------------------------------------------------- */


/* Request to: Allocate a DMA channel
 * ------------------------------------ */
typedef struct
{
	uint32_t src_id;
	uint32_t dst_id;
	pid_t pid;
	int chid;

} t_dma_devctl_request__allocate;

typedef struct
{
	int32_t success;
	uint32_t dma_controller;
	uint32_t dma_channel;
	uint32_t src_peripheral_id;
	uint32_t dst_peripheral_id;

} t_dma_devctl_response__allocate;



/* Request to: Free a DMA channel
 * ------------------------------------ */
typedef struct
{
	uint32_t dma_controller;
	uint32_t dma_channel;
} t_dma_devctl_request__free;

typedef struct
{
	int32_t success;
} t_dma_devctl_response__free;



/* All possible DEVCTLs
 * ------------------------------------ */
typedef struct
{
	union
	{
		t_dma_devctl_request__allocate 	devctl_allocate;
		t_dma_devctl_request__free	 	devctl_free;
	} u;
} t_dma_devctl_request;

typedef struct
{
	t_dma_devctl_request_type request;
	union
	{
		t_dma_devctl_response__allocate 	devctl_allocate;
		t_dma_devctl_response__free	 		devctl_free;
	} u;
} t_dma_devctl_response;

// Wrap request and response in one structure accessible with the same pointer
typedef struct
{
	union
	{
		t_dma_devctl_request 	request;
		t_dma_devctl_response 	response;
	} u;
} t_dma_devctl_req_rsp;

#endif /* DMA_DEVCTL_H_ */
