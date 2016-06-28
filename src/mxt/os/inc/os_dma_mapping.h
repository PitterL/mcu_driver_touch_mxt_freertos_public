/*
 * os_dma_mapping.h
 *
 * Created: 3/12/2016 11:21:03 AM
 *  Author: pitter.liao
 */ 


#ifndef OS_DMA_MAPPING_H_
#define OS_DMA_MAPPING_H_

#include "common/device.h"

typedef uint32_t dma_addr_t;
void *dma_alloc_coherent(struct device *, size_t, dma_addr_t *, unsigned long);
void dma_free_coherent(struct device *, size_t, void *, dma_addr_t);

#endif /* OS_DMA_MAPPING_H_ */