// @nolint
// @generated
struct OatMethod {
  uintptr_t begin_uintptr;
  uintptr_t offset_uintptr;
  bool success_b;
};
struct OatClass {
  uintptr_t oat_file_uintptr;
  intptr_t status_intptr;
  uintptr_t type_uintptr;
  uintptr_t bitmap_size_uintptr;
  uintptr_t bitmap_ptr_uintptr;
  uintptr_t methods_ptr_uintptr;
  bool success_b;
};
struct ArraySlice {
  uintptr_t array_uintptr;
  uintptr_t size_uintptr;
  uintptr_t element_size_uintptr;
};
auto get_runtime_from_thread(uintptr_t thread) {
  uint64_t jni_env = Read8(AccessField(AccessField(thread, 128UL), 56UL));
  uint64_t java_vm = Read8(AccessField(jni_env, 16UL));
  uint64_t runtime = Read8(AccessField(java_vm, 8UL));
  return runtime;
}

auto get_runtime() {
  return get_runtime_from_thread(get_art_thread());
}

auto get_class_dexfile(uintptr_t cls) {
  uintptr_t dexcache_heap_ref = AccessField(cls, 20UL);
  uint32_t dexcache_ptr = Read4(AccessField(dexcache_heap_ref, 0UL));
  uint32_t dexcache = dexcache_ptr;
  uint64_t dexfile = Read8(AccessField(dexcache, 16UL));
  return dexfile;
}

auto get_dexfile_string_by_idx(uintptr_t dexfile, uintptr_t idx) {
  idx = idx;
  uintptr_t id = AccessArrayItem(Read8(AccessField(dexfile, 72UL)), idx, 4UL);
  uint64_t begin = Read8(AccessField(dexfile, 8UL));
  uint32_t string_data_off = Read4(AccessField(id, 0UL));
  uintptr_t ptr = AdvancePointer(begin, (string_data_off * 1UL));
  uintptr_t val = ptr;
  uintptr_t length = 0UL;
  uintptr_t index = 0UL;
  bool proceed = true;
  while (proceed) {
    uint8_t byte = Read1(AccessArrayItem(val, index, 1UL));
    length = (length | ((byte & 127UL) << (index * 7UL)));
    proceed = ((byte & 128UL) != 0UL);
    index = (index + 1UL);
  }
  string_t result =
      String(AdvancePointer(ptr, (index * 1UL)), "ascii", "ignore", length);
  return String(result);
}

auto get_declaring_class(uintptr_t method) {
  uintptr_t declaring_class_gc_root = AccessField(method, 0UL);
  uintptr_t declaring_class_ref = AccessField(declaring_class_gc_root, 0UL);
  uint32_t declaring_class_ptr = Read4(AccessField(declaring_class_ref, 0UL));
  uint32_t declaring_class = declaring_class_ptr;
  return declaring_class;
}

auto get_method_trace_id(uintptr_t method) {
  auto cls = get_declaring_class(method);
  auto dexfile = get_class_dexfile(cls);
  uintptr_t signature = AccessField(Read8(AccessField(dexfile, 64UL)), 12UL);
  uint32_t dex_id = Read4(signature);
  dex_id = dex_id;
  uint32_t method_id = Read4(AccessField(method, 12UL));
  return GetMethodTraceId(dex_id, method_id);
}

auto get_method_name(uintptr_t method) {
  auto cls = get_declaring_class(method);
  auto dexfile = get_class_dexfile(cls);
  uint32_t dex_method_index = Read4(AccessField(method, 12UL));
  uintptr_t method_id =
      AccessArrayItem(Read8(AccessField(dexfile, 96UL)), dex_method_index, 8UL);
  uint32_t name_idx = Read4(AccessField(method_id, 4UL));
  return get_dexfile_string_by_idx(dexfile, name_idx);
}

auto get_class_descriptor(uintptr_t cls) {
  auto dexfile = get_class_dexfile(cls);
  uint32_t typeidx = Read4(AccessField(cls, 96UL));
  uintptr_t typeid_ =
      AccessArrayItem(Read8(AccessField(dexfile, 80UL)), typeidx, 4UL);
  uint32_t descriptor_idx = Read4(AccessField(typeid_, 0UL));
  return get_dexfile_string_by_idx(dexfile, descriptor_idx);
}

auto get_method_shorty(uintptr_t method) {
  auto cls = get_declaring_class(method);
  auto dexfile = get_class_dexfile(cls);
  uint32_t dex_method_index = Read4(AccessField(method, 12UL));
  uintptr_t method_id =
      AccessArrayItem(Read8(AccessField(dexfile, 96UL)), dex_method_index, 8UL);
  uint16_t proto_idx = Read2(AccessField(method_id, 2UL));
  uintptr_t method_proto_id =
      AccessArrayItem(Read8(AccessField(dexfile, 104UL)), proto_idx, 12UL);
  uint32_t shorty_id = Read4(AccessField(method_proto_id, 0UL));
  return get_dexfile_string_by_idx(dexfile, shorty_id);
}

auto get_number_of_refs_without_receiver(uintptr_t method) {
  auto shorty = get_method_shorty(method);
  return CountShortyRefs(shorty);
}

auto get_method_access_flags(uintptr_t method) {
  uint32_t access_flags = Read4(AccessField(method, 4UL));
  return access_flags;
}

auto is_runtime_method(uintptr_t method) {
  uint32_t dex_method_index = Read4(AccessField(method, 12UL));
  bool is_runtime_method = (dex_method_index == 4294967295UL);
  return is_runtime_method;
}

auto is_proxy_method(uintptr_t method) {
  auto declaring_class = get_declaring_class(method);
  uint32_t class_access_flags = Read4(AccessField(declaring_class, 44UL));
  uintptr_t kAccClassIsProxy = 262144UL;
  if ((class_access_flags & kAccClassIsProxy)) {
    return true;
  } else {
    return false;
  }
}

auto is_static_method(uintptr_t method) {
  uintptr_t kAccStatic = 8UL;
  if ((get_method_access_flags(method) & kAccStatic)) {
    return true;
  } else {
    return false;
  }
}

auto is_direct_method(uintptr_t method) {
  uintptr_t kAccStatic = 8UL;
  uintptr_t kAccPrivate = 2UL;
  uintptr_t kAccConstructor = 65536UL;
  if ((get_method_access_flags(method) &
       ((kAccStatic | kAccPrivate) | kAccConstructor))) {
    return true;
  } else {
    return false;
  }
}

auto is_native_method(uintptr_t method) {
  uintptr_t kAccNative = 256UL;
  if ((get_method_access_flags(method) & kAccNative)) {
    return true;
  } else {
    return false;
  }
}

auto is_quick_resolution_stub(
    uintptr_t entry_point,
    uintptr_t runtime,
    uintptr_t thread) {
  uint64_t class_linker = Read8(AccessField(runtime, 400UL));
  uintptr_t entry_points = AccessField(AccessField(thread, 128UL), 288UL);
  return (
      (Read8(AccessField(class_linker, 368UL)) == entry_point) ||
      (Read8(AccessField(entry_points, 760UL)) == entry_point));
}

auto is_quick_to_interpreter_bridge(
    uintptr_t entry_point,
    uintptr_t runtime,
    uintptr_t thread) {
  uint64_t class_linker = Read8(AccessField(runtime, 400UL));
  uintptr_t entry_points = AccessField(AccessField(thread, 128UL), 288UL);
  return (
      (Read8(AccessField(class_linker, 392UL)) == entry_point) ||
      (Read8(AccessField(entry_points, 768UL)) == entry_point));
}

auto is_quick_generic_jni_stub(
    uintptr_t entry_point,
    uintptr_t runtime,
    uintptr_t thread) {
  uint64_t class_linker = Read8(AccessField(runtime, 400UL));
  uintptr_t entry_points = AccessField(AccessField(thread, 128UL), 288UL);
  return (
      (Read8(AccessField(class_linker, 384UL)) == entry_point) ||
      (Read8(AccessField(entry_points, 416UL)) == entry_point));
}

auto get_quick_entry_point_from_compiled_code(uintptr_t method) {
  uintptr_t ptr_fields = AccessField(method, 20UL);
  uint64_t entry_point = Read8(AccessField(ptr_fields, 24UL));
  entry_point = entry_point;
  return entry_point;
}

auto get_oat_method_header_from_entry_point(uintptr_t entry_point) {
  entry_point = entry_point;
  entry_point = (entry_point & (~1UL));
  uintptr_t header_offset = 20UL;
  uintptr_t oat_method_header = (entry_point - header_offset);
  return oat_method_header;
}

auto get_quick_frame_info_from_entry_point(uintptr_t entry_point) {
  auto oat_method_header = get_oat_method_header_from_entry_point(entry_point);
  return AccessField(oat_method_header, 4UL);
}

auto method_header_contains(uintptr_t method_header, uintptr_t pc) {
  uintptr_t code = AccessField(method_header, 20UL);
  uint32_t code_size = Read4(AccessField(method_header, 16UL));
  return ((code <= pc) && (pc <= (code + code_size)));
}

auto is_resolved(uintptr_t cls) {
  uintptr_t status = AccessField(cls, 120UL);
  uintptr_t kStatusResolved = 4UL;
  intptr_t kStatusErrorResolved = (-2UL);
  return (status >= kStatusResolved);
}

auto get_oat_class(uintptr_t oat_dex_file, uintptr_t class_def_idx) {
  uint64_t oat_class_offsets_pointer = Read8(AccessField(oat_dex_file, 80UL));
  uintptr_t oat_class_offset =
      AdvancePointer(oat_class_offsets_pointer, (class_def_idx * 4UL));
  oat_class_offset = Read4(oat_class_offset);
  uint64_t oat_file = Read8(AccessField(oat_dex_file, 0UL));
  uint64_t oat_file_begin = Read8(AccessField(oat_file, 32UL));
  uintptr_t oat_class_pointer =
      AdvancePointer(oat_file_begin, (oat_class_offset * 1UL));

  uintptr_t status_pointer = oat_class_pointer;
  uint16_t status = Read2(status_pointer);
  uintptr_t kStatusMax = 12UL;

  uintptr_t type_pointer = AdvancePointer(status_pointer, (2UL * 1UL));

  uint16_t oat_type = Read2(type_pointer);
  uintptr_t kOatClassMax = 3UL;

  uintptr_t after_type_pointer = AdvancePointer(type_pointer, (2UL * 1UL));

  uintptr_t bitmap_size = 0UL;
  uintptr_t bitmap_pointer = 0UL;
  uintptr_t methods_pointer = 0UL;
  uintptr_t kOatClassNoneCompiled = 2UL;
  if ((oat_type != kOatClassNoneCompiled)) {
    uintptr_t kOatClassSomeCompiled = 1UL;
    if ((oat_type == kOatClassSomeCompiled)) {
      bitmap_size = Read4(after_type_pointer);
      bitmap_pointer = AdvancePointer(after_type_pointer, (4UL * 1UL));

      methods_pointer = AdvancePointer(bitmap_pointer, (bitmap_size * 1UL));
    } else {
      methods_pointer = after_type_pointer;
    }
  }
  return OatClass{
      .oat_file_uintptr = oat_file,
      .status_intptr = status,
      .type_uintptr = oat_type,
      .bitmap_size_uintptr = bitmap_size,
      .bitmap_ptr_uintptr = bitmap_pointer,
      .methods_ptr_uintptr = methods_pointer,
      .success_b = true,
  };
}

auto find_oat_class(uintptr_t cls) {
  auto dex_file = get_class_dexfile(cls);
  uint32_t class_def_idx = Read4(AccessField(cls, 92UL));
  uintptr_t kDexNoIndex16 = 65535UL;

  uint64_t oat_dex_file = Read8(AccessField(dex_file, 120UL));
  if (((oat_dex_file == 0UL) ||
       (Read8(AccessField(oat_dex_file, 0UL)) == 0UL))) {
    return OatClass{
        .oat_file_uintptr = 0,
        .status_intptr = -1,
        .type_uintptr = 2,
        .bitmap_size_uintptr = 0,
        .bitmap_ptr_uintptr = 0,
        .methods_ptr_uintptr = 0,
        .success_b = false,
    };
  } else {
    return get_oat_class(oat_dex_file, class_def_idx);
  }
}

auto count_bits_in_word(uintptr_t word) {
  uintptr_t count = 0UL;
  while ((word > 0UL)) {
    if ((word & 1UL)) {
      count = (count + 1UL);
    }
    word = (word >> 1UL);
  }
  return count;
}

auto get_oat_method_offsets(
    struct OatClass const& struct_OatClass,
    uintptr_t method_index) {
  uintptr_t methods_ptr = struct_OatClass.methods_ptr_uintptr;
  uintptr_t oc_type = struct_OatClass.type_uintptr;
  uintptr_t bitmap_ptr = struct_OatClass.bitmap_ptr_uintptr;
  if ((methods_ptr == 0UL)) {
    uintptr_t kOatClassNoneCompiled = 2UL;

    return methods_ptr;
  }
  uintptr_t methods_pointer_index = 0UL;
  if ((bitmap_ptr == 0UL)) {
    uintptr_t kOatClassAllCompiled = 0UL;

    methods_pointer_index = method_index;
  } else {
    uintptr_t kOatClassSomeCompiled = 1UL;

    uintptr_t word_index = (method_index >> 5UL);
    uintptr_t bit_mask = (1UL << (method_index & 31UL));
    uintptr_t is_bit_set = AdvancePointer(bitmap_ptr, (word_index * 4UL));
    is_bit_set = Read4(is_bit_set);
    is_bit_set = ((is_bit_set & bit_mask) != 0UL);
    if ((!is_bit_set)) {
      return is_bit_set;
    }
    uintptr_t word_end = (method_index >> 5UL);
    uintptr_t partial_word_bits = (method_index & 31UL);
    uintptr_t count = 0UL;
    uintptr_t word = 0UL;
    uintptr_t elem = 0UL;
    while ((word < word_end)) {
      elem = AdvancePointer(bitmap_ptr, (word * 4UL));
      elem = Read4(elem);
      count = (count + count_bits_in_word(elem));
      word = (word + 1UL);
    }
    if ((partial_word_bits != 0UL)) {
      elem = AdvancePointer(bitmap_ptr, (word_end * 4UL));
      elem = Read4(elem);
      uint32_t shifted = 4294967295U;
      count =
          (count +
           count_bits_in_word((elem & (~(shifted << partial_word_bits)))));
    }
    methods_pointer_index = count;
  }
  uintptr_t ret = AdvancePointer(methods_ptr, (methods_pointer_index * 4UL));
  return ret;
}

auto runtime_is_aot_compiler(uintptr_t runtime, uintptr_t instance) {
  uint64_t jit =
      Read8(AccessField(AccessField(AccessField(runtime, 448UL), 0UL), 0UL));
  bool use_jit_compilation = ((jit != 0UL) && Read1(AccessField(jit, 360UL)));
  uint64_t compiler_callbacks = Read8(AccessField(runtime, 104UL));
  return ((!use_jit_compilation) && (compiler_callbacks != 0UL));
}

auto get_oat_method(
    uintptr_t runtime_obj,
    struct OatClass const& struct_OatClass,
    uintptr_t oat_method_index) {
  auto oat_method_offsets =
      get_oat_method_offsets(struct_OatClass, oat_method_index);
  if ((oat_method_offsets == 0UL)) {
    return OatMethod{
        .begin_uintptr = 0,
        .offset_uintptr = 0,
        .success_b = true,
    };
  }
  uintptr_t runtime_current = get_runtime();
  uintptr_t begin_uintptr = 0UL;
  uintptr_t oat_file = struct_OatClass.oat_file_uintptr;
  if ((Read1(AccessField(oat_file, 64UL)) ||
       ((runtime_current == 0UL) ||
        runtime_is_aot_compiler(runtime_current, runtime_current)))) {
    begin_uintptr = Read8(AccessField(oat_file, 32UL));
    uint32_t offset_uintptr = Read4(AccessField(oat_method_offsets, 0UL));
    return OatMethod{
        .begin_uintptr = begin_uintptr,
        .offset_uintptr = offset_uintptr,
        .success_b = true,
    };
  }
  begin_uintptr = Read8(AccessField(oat_file, 32UL));
  return OatMethod{
      .begin_uintptr = begin_uintptr,
      .offset_uintptr = 0,
      .success_b = true,
  };
}

auto round_up(uintptr_t x, uintptr_t n) {
  uintptr_t arg1 = ((x + n) - 1UL);
  uintptr_t arg2 = n;
  return (arg1 & (-arg2));
}

auto length_prefixed_array_at(
    uintptr_t array,
    uintptr_t idx,
    uintptr_t element_size,
    uintptr_t alignment) {
  uintptr_t ptr = array;

  uintptr_t data_offset = 4UL;
  auto element_offset = round_up(data_offset, alignment);
  element_offset = (element_offset + (idx * element_size));
  uintptr_t ret = (ptr + element_offset);
  return ret;
}

auto get_virtual_methods(
    uintptr_t method,
    uintptr_t cls,
    uintptr_t start_offset) {
  uintptr_t ptr_size = 8UL;
  uintptr_t num_methods = 0UL;
  uint64_t methods_ptr = Read8(AccessField(cls, 64UL));
  if ((methods_ptr == 0UL)) {
    num_methods = 0UL;
  } else {
    num_methods = Read4(AccessField(methods_ptr, 0UL));
  }
  uintptr_t end_offset = num_methods;

  uintptr_t size = (end_offset - start_offset);
  if ((size == 0UL)) {
    return ArraySlice{
        .array_uintptr = 0,
        .size_uintptr = 0,
        .element_size_uintptr = 0,
    };
  }

  uintptr_t method_size = 20UL;
  method_size = round_up(method_size, ptr_size);
  method_size = (method_size + 32UL);
  uintptr_t method_alignment = ptr_size;
  auto array_uintptr = length_prefixed_array_at(
      methods_ptr, start_offset, method_size, method_alignment);
  return ArraySlice{
      .array_uintptr = array_uintptr,
      .size_uintptr = size,
      .element_size_uintptr = method_size,
  };
  auto array_method =
      length_prefixed_array_at(methods_ptr, 0UL, method_size, method_alignment);
  uint32_t size_uintptr = Read4(AccessField(methods_ptr, 0UL));
  auto array_slice = ArraySlice{
      .array_uintptr = array_method,
      .size_uintptr = size_uintptr,
      .element_size_uintptr = method_size,
  };

  uintptr_t tmp = array_slice.array_uintptr;
  tmp = (tmp + (start_offset * array_slice.element_size_uintptr));
  return ArraySlice{
      .array_uintptr = tmp,
      .size_uintptr = size,
      .element_size_uintptr = array_slice.element_size_uintptr,
  };
}

auto find_oat_method_for(uintptr_t method, uintptr_t runtime_obj) {
  uintptr_t oat_method_index = 0UL;
  auto cls = get_declaring_class(method);
  if ((is_static_method(method) || is_direct_method(method))) {
    oat_method_index = Read2(AccessField(method, 16UL));
  } else {
    oat_method_index = Read2(AccessField(cls, 126UL));
    auto virtual_methods = get_virtual_methods(method, cls, oat_method_index);
    uintptr_t iterator = virtual_methods.array_uintptr;
    uintptr_t end =
        (iterator +
         (virtual_methods.size_uintptr * virtual_methods.element_size_uintptr));
    bool found_virtual = false;
    while ((iterator != end)) {
      uintptr_t art_method = iterator;
      if ((Read4(AccessField(art_method, 12UL)) ==
           Read4(AccessField(method, 12UL)))) {
        found_virtual = true;
        break;
      }
      oat_method_index = (oat_method_index + 1UL);
      iterator = (iterator + virtual_methods.element_size_uintptr);
    }
  }
  auto oat_class = find_oat_class(cls);
  if ((!oat_class.success_b)) {
    return OatMethod{
        .begin_uintptr = 0,
        .offset_uintptr = 0,
        .success_b = false,
    };
  }
  return get_oat_method(runtime_obj, oat_class, oat_method_index);
}

auto get_oat_pointer(
    struct OatMethod const& struct_OatMethod,
    uintptr_t offset) {
  if ((offset == 0UL)) {
    return offset;
  }
  uintptr_t begin = struct_OatMethod.begin_uintptr;
  return AdvancePointer(begin, (offset * 1UL));
}

auto get_code_offset(struct OatMethod const& struct_OatMethod) {
  uintptr_t oat_method_offset = struct_OatMethod.offset_uintptr;
  auto code = get_oat_pointer(struct_OatMethod, oat_method_offset);
  code = (code & (~1UL));
  if ((code == 0UL)) {
    return 0UL;
  }
  uintptr_t method_header_size = 20UL;
  code = (code - method_header_size);
  uintptr_t kCodeSizeMask = (~2147483648UL);
  uint32_t code_size = Read4(AccessField(code, 16UL));
  if ((code_size == 0UL)) {
    return 0UL;
  }
  return oat_method_offset;
}

auto get_quick_code(struct OatMethod const& struct_OatMethod) {
  auto offset = get_code_offset(struct_OatMethod);
  auto quick_code = get_oat_pointer(struct_OatMethod, offset);
  return quick_code;
}

auto get_oat_quick_method_header(
    uintptr_t method,
    uintptr_t runtime_obj,
    uintptr_t thread_obj,
    uintptr_t pc) {
  if (is_runtime_method(method)) {
    return 0UL;
  }
  auto existing_entry_point = get_quick_entry_point_from_compiled_code(method);

  if (is_quick_generic_jni_stub(
          existing_entry_point, runtime_obj, thread_obj)) {
    return 0UL;
  }
  uintptr_t method_header = 0UL;
  if (((!is_quick_generic_jni_stub(
           existing_entry_point, runtime_obj, thread_obj)) &&
       (!is_quick_resolution_stub(
           existing_entry_point, runtime_obj, thread_obj)) &&
       (!is_quick_to_interpreter_bridge(
           existing_entry_point, runtime_obj, thread_obj)))) {
    auto entry_point_tmp = existing_entry_point;
    method_header = get_oat_method_header_from_entry_point(entry_point_tmp);
    if (method_header_contains(method_header, pc)) {
      return method_header;
    }
  }
  auto oat_method = find_oat_method_for(method, runtime_obj);
  if ((!oat_method.success_b)) {
    if (is_quick_resolution_stub(
            existing_entry_point, runtime_obj, thread_obj)) {
      return 0UL;
    }
  }
  auto oat_entry_point = get_quick_code(oat_method);
  if (((oat_entry_point == 0UL) ||
       is_quick_generic_jni_stub(oat_entry_point, runtime_obj, thread_obj))) {
    return 0UL;
  }
  oat_entry_point = oat_entry_point;
  method_header = get_oat_method_header_from_entry_point(oat_entry_point);
  if ((pc == 0UL)) {
    return method_header;
  }
  return method_header;
}

auto is_abstract_method(uintptr_t method) {
  uintptr_t kAccAbstract = 1024UL;
  return (get_method_access_flags(method) & kAccAbstract);
}

auto get_frame_size(
    uintptr_t frameptr,
    uintptr_t runtime_obj,
    uintptr_t thread_obj,
    uintptr_t pc) {
  uintptr_t method = frameptr;
  auto entry_point = get_quick_entry_point_from_compiled_code(method);
  auto oat_quick_method_header =
      get_oat_quick_method_header(method, runtime_obj, thread_obj, pc);
  if ((oat_quick_method_header != 0UL)) {
    return Read4(AccessField(AccessField(oat_quick_method_header, 4UL), 0UL));
  }
  uint32_t size = 0U;
  uintptr_t callee_save_methods = AccessField(runtime_obj, 0UL);
  uintptr_t callee_save_infos = AccessField(runtime_obj, 64UL);
  uintptr_t kSaveAll = 0UL;
  uintptr_t kRefsOnly = 1UL;
  uintptr_t kRefsAndArgs = 2UL;
  uintptr_t method_info = 0UL;
  if (is_abstract_method(method)) {
    method_info = AccessArrayItem(callee_save_infos, kRefsAndArgs, 12UL);
    size = Read4(AccessField(method_info, 0UL));
    return size;
  }
  if (is_runtime_method(method)) {
    if ((frameptr ==
         Read8(AccessArrayItem(callee_save_methods, kRefsAndArgs, 8UL)))) {
      method_info = AccessArrayItem(callee_save_infos, kRefsAndArgs, 12UL);
    } else {
      if ((frameptr ==
           Read8(AccessArrayItem(callee_save_methods, kSaveAll, 8UL)))) {
        method_info = AccessArrayItem(callee_save_infos, kSaveAll, 12UL);
      } else {
        method_info = AccessArrayItem(callee_save_infos, kRefsOnly, 12UL);
      }
    }
    size = Read4(AccessField(method_info, 0UL));
    return size;
  }
  if (is_proxy_method(method)) {
    if (is_direct_method(method)) {
      auto info = get_quick_frame_info_from_entry_point(entry_point);
      size = Read4(AccessField(info, 0UL));
      return size;
    } else {
      method_info = AccessArrayItem(callee_save_infos, kRefsAndArgs, 12UL);
      size = Read4(AccessField(method_info, 0UL));
      return size;
    }
  }
  uintptr_t code = 0UL;
  bool is_native = false;
  if ((is_quick_resolution_stub(entry_point, runtime_obj, thread_obj) ||
       is_quick_to_interpreter_bridge(entry_point, runtime_obj, thread_obj))) {
    if (is_native_method(method)) {
      is_native = true;
    } else {
      ;
    }
  }
  code = entry_point;
  if ((is_native || is_quick_generic_jni_stub(code, runtime_obj, thread_obj))) {
    uintptr_t callee_info =
        AccessArrayItem(callee_save_infos, kRefsAndArgs, 12UL);
    uint32_t callee_info_size = Read4(AccessField(callee_info, 0UL));
    uintptr_t voidptr_size = 8UL;
    uintptr_t artmethodptr_size = 8UL;
    auto num_refs = (get_number_of_refs_without_receiver(method) + 1UL);
    uintptr_t handle_scope_size = (12UL + (4UL * num_refs));
    size =
        (((callee_info_size - voidptr_size) + artmethodptr_size) +
         handle_scope_size);
    uintptr_t kStackAlignment = 16UL;
    size = round_up(size, kStackAlignment);
    return size;
  }
  auto frame_info = get_quick_frame_info_from_entry_point(code);
  size = Read4(AccessField(frame_info, 0UL));
  return size;
}

auto unwind(unwind_callback_t __unwind_callback, void* __unwind_data) {
  uintptr_t thread = get_art_thread();
  if ((thread == 0UL)) {
    return true;
  }
  auto runtime = get_runtime_from_thread(thread);
  uintptr_t thread_obj = thread;
  auto runtime_obj = runtime;
  uintptr_t tls = AccessField(thread_obj, 128UL);
  uintptr_t mstack = AccessField(tls, 24UL);
  uint64_t generic_jni_trampoline =
      Read8(AccessField(AccessField(tls, 288UL), 416UL));
  while ((mstack != 0UL)) {
    uint64_t quick_frame = Read8(AccessField(mstack, 0UL));
    quick_frame = quick_frame;
    uint64_t shadow_frame = Read8(AccessField(mstack, 16UL));
    shadow_frame = shadow_frame;
    uintptr_t pc = 0UL;
    uintptr_t kMaxFrames = 1024UL;
    uintptr_t depth = 0UL;
    if ((quick_frame != 0UL)) {
      while (((quick_frame != 0UL) && (depth < kMaxFrames))) {
        uint64_t frameptr = Read8(quick_frame);
        if ((frameptr == 0UL)) {
          break;
        }
        uint64_t frame = frameptr;
        if ((!is_runtime_method(frame))) {
          if ((!__unwind_callback(frame, __unwind_data))) {
            return false;
          }
        }
        auto size = get_frame_size(frameptr, runtime_obj, thread_obj, pc);
        auto return_pc_offset = (size - 8UL);
        uint64_t return_pc_addr = (quick_frame + return_pc_offset);
        uint64_t return_pc = return_pc_addr;
        pc = Read8(return_pc);
        quick_frame = (quick_frame + size);
        depth = (depth + 1UL);
      }
    } else {
      if ((shadow_frame != 0UL)) {
        while (((shadow_frame != 0UL) && (depth < kMaxFrames))) {
          uint64_t frame_obj = shadow_frame;
          uint64_t artmethodptr = Read8(AccessField(frame_obj, 8UL));
          uint64_t artmethod = artmethodptr;
          if ((!is_runtime_method(artmethod))) {
            if ((!__unwind_callback(artmethod, __unwind_data))) {
              return false;
            }
          }
          shadow_frame = Read8(AccessField(frame_obj, 0UL));
          depth = (depth + 1UL);
        }
      }
    }
    uint64_t link = Read8(AccessField(mstack, 8UL));
    if ((link == 0UL)) {
      break;
    }
    mstack = link;
  }
  return true;
}
