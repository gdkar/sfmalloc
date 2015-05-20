
#ifndef INCLUDE_LOL_DEBUG
#define INCLUDE_LOL_DEBUG
// Output stream for debugging
static FILE *g_DOUT = NULL;

static void debug_init() {
  g_DOUT = stdout;
}
static inline void print_class_array() {
  fprintf(g_DOUT, "========== SizeMap.class_array ==========\n");
  for (uint32_t i = 0; i < CLASS_ARRAY_SIZE; ++i) {
    fprintf(g_DOUT, "%3d: %u\n", i, g_sizemap.class_array[i]);
  }
  fprintf(g_DOUT, "\n");
}
static inline void print_class_to_size() {
  fprintf(g_DOUT, "========== SizeMap.class_to_size ==========\n");
  for (uint32_t i = 0; i < NUM_CLASSES; ++i) {
    fprintf(g_DOUT, "%2d: %u (%u)\n",
        i, g_sizemap.info[i].class_to_size,
        g_sizemap.info[i].class_to_size % CACHE_LINE_SIZE);
  }
  fprintf(g_DOUT, "\n");
}
static inline void print_class_to_pages() {
  fprintf(g_DOUT, "========== SizeMap.class_to_pages ==========\n");
  for (uint32_t i = 0; i < NUM_CLASSES; ++i) {
    fprintf(g_DOUT, "%2d: %u\n", i, g_sizemap.info[i].class_to_pages);
  }
  fprintf(g_DOUT, "\n");
}
static inline void print_num_blocks_per_pbh() {
  fprintf(g_DOUT, "========== SizeMap.num_blocks_per_pbh ==========\n");
  for (int i = 0; i < NUM_CLASSES; ++i) {
    fprintf(g_DOUT, "%2d: %u\n", i, g_sizemap.info[i].num_blocks_per_pbh);
  }
  fprintf(g_DOUT, "\n");
}
#ifdef MALLOC_DEBUG_DETAIL
static void print_sizemap() {
  print_class_array();
  print_class_to_size();
  print_class_to_pages();
  print_num_blocks_per_pbh();
}
#endif
uint32_t get_pbh_list_length(const pbh_t* list) {
  if (list == NULL) return 0;
  uint32_t len = 1;
  for (pbh_t* s = list->next; s != list; s = s->next) {len++;}
  return len;
}
static const char* get_pbh_status_str(uint32_t status) {
  switch (status) {
    case PBH_ON_FREE_LIST:          return "PBH_ON_FREE_LIST";
    case PBH_IN_USE:                return "PBH_IN_USE";
    case PBH_AGAINST_FALSE_SHARING: return "PBH_AGAINST_FALSE_SHARING";
    default:                        return "UNKNOWN";
  }
}
void print_pbh(pbh_t* pbh) {
  fprintf(g_DOUT,
      "---------------------------------------\n"
      "current pbh: %p [T%u]\n"
      "---------------------------------------\n"
      "next        : %p\n"
      "prev        : %p\n"
      "start_page  : 0x%lx\n"
      "length      : %u\n"
      "sizeclass   : %u\n"
      "status      : %s\n"
      "cnt_free    : %u\n"
      "cnt_unused  : %u\n"
      "page_color  : %u\n"
      "block_color : %u\n"
      "free_list   : %p\n"
      "unallocated : %p\n"
      "remote_list.head : %u\n"
      "remote_list.cnt  : %u\n"
      "---------------------------------------\n",
      pbh, l_tlh.thread_id,
      pbh->next, pbh->prev,
      pbh->start_page, pbh->length, pbh->sizeclass,
      get_pbh_status_str(pbh->status),
      pbh->cnt_free, pbh->cnt_unused,
      pbh->page_color, pbh->block_color,
      pbh->free_list, pbh->unallocated,
      pbh->remote_list.head, pbh->remote_list.cnt
  );
}
void print_pbh_list(pbh_t* list) {
  fprintf(g_DOUT, "========== PBH List ==========\n"); 
  if (list == NULL) {
    fprintf(g_DOUT, "No list\n");
    return;
  }
  uint64_t sum_cnt_free = 0;
  uint64_t sum_cnt_unused = 0;
  uint64_t sum_cnt_remote = 0;
  pbh_t* pbh = list;
  while (1) {
    print_pbh(pbh);
    sum_cnt_free += pbh->cnt_free;
    sum_cnt_unused += pbh->cnt_unused;
    sum_cnt_remote += pbh->remote_list.cnt;
    fprintf(g_DOUT, "PBH FREE LIST: \n");
    print_block_list(pbh->free_list);
    pbh = pbh->next;
    if (pbh != list) fprintf(g_DOUT, "--->\n");
    else break;
    if (pbh == pbh->next) {
      fprintf(g_DOUT, "!!!!!!!! WHAT? !!!!!!!!\n");
      break;
    }
  };
  fprintf(g_DOUT,
      "---------------------------------------\n"
      "SUMMARY\n"
      "---------------------------------------\n"
      "length of list: %u\n"
      "sum_cnt_free  : %lu\n"
      "sum_cnt_unused: %lu\n"
      "sum_cnt_remote: %lu\n"
      "---------------------------------------\n\n",
      get_pbh_list_length(list),
      sum_cnt_free, sum_cnt_unused, sum_cnt_remote
  );
  fflush(g_DOUT);
}
void print_superpage(sph_t* spage) {
  fprintf(g_DOUT,
      "---------------------------------------\n"
      "current SP : %p\n"
      "---------------------------------------\n"
      "next       : %p\n"
      "prev       : %p\n"
      "start_page : 0x%lx\n"
      "omark.onwer_id    : %u\n"
      "omark.finish_mark : %u\n"
      "---------------------------------------\n",
      spage,
      spage->next, spage->prev, spage->start_page,
      spage->omark.owner_id, spage->omark.finish_mark
  );

  pbh_t* pbh = (pbh_t*)spage + 1;
  uint32_t total_len = 0;
  while (total_len < SUPERPAGE_LEN) {
    uint32_t len = pbh->length;
    print_pbh(pbh);

    // next pbh
    total_len += len;
    pbh = pbh + len;
  }
  assert(total_len == SUPERPAGE_LEN);
}

void print_superpage_list(sph_t* list) {
  fprintf(g_DOUT, "========== SuperPage List ==========\n"); 
  if (list == NULL) {
    fprintf(g_DOUT, "No list\n");
    return;
  }
  uint32_t superpage_len = 1;
  sph_t* spage = list;
  while (1) {
    print_superpage(spage);
    // next superpage
    spage = spage->next;
    if (spage != list) {
      fprintf(g_DOUT, "--->\n");
      superpage_len++;
    } else break;
  };
  fprintf(g_DOUT,
      "---------------------------------------\n"
      "SUMMARY\n"
      "---------------------------------------\n"
      "length of list: %u\n"
      "---------------------------------------\n\n",
      superpage_len
  );
  fflush(g_DOUT);
}
void print_free_pb_list(tlh_t* tlh) {
  fprintf(g_DOUT, "========== Free Page Block Lists ==========\n"); 
  size_t pagelist_cnt = 0;
  for (uint32_t i = 0; i < NUM_PAGE_CLASSES; i++) {
    size_t length = get_pbh_list_length(tlh->free_pb_list[i]);
    if (length == 0) continue;
    fprintf(g_DOUT,
        "---------------------------------------\n"
        "FPBL %u| Length: %lu\n"
        "---------------------------------------\n",
        i, length
    );
    print_pbh_list(tlh->free_pb_list[i]);
    pagelist_cnt += length;
  }
  fprintf(g_DOUT,
      "---------------------------------------\n"
      "FPBL SUMMARY\n"
      "---------------------------------------\n"
      "Total #: %lu\n"
      "---------------------------------------\n\n",
      pagelist_cnt
  );
  fflush(g_DOUT);
}
static uint32_t get_block_list_length(void* block) {
  uint32_t length = 0;
  while (block != NULL) {
    length++;
    block = GET_NEXT(block);
  }
  return length;
}
void print_block_list(void* block) {
  uint32_t length = 0;
  while (block != NULL) {
    length++;
    fprintf(g_DOUT, "%p --> ", block);
    if (length % 5 == 0) fprintf(g_DOUT, "\n");
    block = GET_NEXT(block);
  }
  fprintf(g_DOUT, "%p\n", block);
}
#ifdef MALLOC_USE_PAGE_BLOCK_CACHE
static void print_pb_cache(pb_cache_t* pb_cache) {
  fprintf(g_DOUT, "========== Page Block Cache ==========\n"); 
  for (int w = 0; w < NUM_PB_CACHE_WAY; w++) {
    fprintf(g_DOUT, "%d: ", w);
    pb_cache_block_t* way = &pb_cache->block[w];
    uint32_t length = 0;
    void* block = way->data;
    while (block != NULL) {
      length++;
      size_t page_id = (size_t)block >> PAGE_SHIFT;
      pbh_t* pbh = (pbh_t*)pagemap_get(page_id);

      fprintf(g_DOUT, "%p(%u) --> ", block, pbh->length);
      if (length % 5 == 0) fprintf(g_DOUT, "\n   ");
      block = GET_NEXT(block);
    }
    fprintf(g_DOUT, "%p\n", block);
  }
  fprintf(g_DOUT, "\n");
}
#else
#define print_pb_cache(c)
#endif

void print_tlh(tlh_t* tlh) {
  char thread_name[64];
  pthread_getname_np(pthread_self(),thread_name,sizeof(thread_name));
  fprintf(g_DOUT, "========== Thread Local Heap [T%u](%s) ==========\n", tlh->thread_id,thread_name); 
  fprintf(g_DOUT, "========== Block Lists ==========\n"); 
  for (uint32_t i = 0; i < NUM_CLASSES; i++) {
    blk_list_t* b_list = &tlh->blk_list[i];
    if ((b_list->pbh_list == NULL) &&
        (b_list->free_blk_list == NULL) &&
        (b_list->ptr_to_unused == NULL) &&
        (b_list->cnt_free == 0) &&
        (b_list->cnt_unused == 0)) continue;
    fprintf(g_DOUT,
          "---------------------------------------\n"
          "BLOCK LIST %u\n"
          "---------------------------------------\n"
          "free_blk_list : %p (len:%u)\n"
          "unallocated   : %p\n"
          "cnt_free      : %u\n"
          "cnt_unused    : %u\n"
          "pbh_list      : %p (len:%u)\n"
          "---------------------------------------\n",
          i,
          b_list->free_blk_list, get_block_list_length(b_list->free_blk_list),
          b_list->ptr_to_unused,
          b_list->cnt_free,
          b_list->cnt_unused,
          b_list->pbh_list, get_pbh_list_length(b_list->pbh_list)
        );
    print_pbh_list(b_list->pbh_list);
    fprintf(g_DOUT, "---------------------------------------\n");
    fprintf(g_DOUT, "FREE LIST:\n");
    print_block_list(b_list->free_blk_list);
    fprintf(g_DOUT, "---------------------------------------\n");
  }
  fprintf(g_DOUT, "\n");
  print_free_pb_list(tlh);
  print_pb_cache(&tlh->pb_cache);
  print_superpage_list(tlh->sp_list);
  fprintf(g_DOUT, "\n");
}
#endif // INCLUDE_LOL_DEBUG

