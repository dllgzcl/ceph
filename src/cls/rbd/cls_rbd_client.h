// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab

#ifndef CEPH_LIBRBD_CLS_RBD_CLIENT_H
#define CEPH_LIBRBD_CLS_RBD_CLIENT_H

#include "cls/lock/cls_lock_types.h"
#include "cls/rbd/cls_rbd_types.h"
#include "common/bit_vector.hpp"
#include "common/snap_types.h"
#include "include/rados/librados.hpp"
#include "include/types.h"
#include "librbd/parent_types.h"

#include <string>
#include <vector>

class Context;

namespace librbd {
  namespace cls_client {
    // high-level interface to the header
    int get_immutable_metadata(librados::IoCtx *ioctx, const std::string &oid,
			       std::string *object_prefix, uint8_t *order);
    int get_mutable_metadata(librados::IoCtx *ioctx, const std::string &oid,
			     bool read_only, uint64_t *size, uint64_t *features,
			     uint64_t *incompatible_features,
			     map<rados::cls::lock::locker_id_t,
				 rados::cls::lock::locker_info_t> *lockers,
			     bool *exclusive_lock,
			     std::string *lock_tag,
			     ::SnapContext *snapc,
			     parent_info *parent);

    // low-level interface (mainly for testing)
    int create_image(librados::IoCtx *ioctx, const std::string &oid,
		     uint64_t size, uint8_t order, uint64_t features,
		     const std::string &object_prefix);
    int get_features(librados::IoCtx *ioctx, const std::string &oid,
		     snapid_t snap_id, uint64_t *features);
    int set_features(librados::IoCtx *ioctx, const std::string &oid,
                     uint64_t features, uint64_t mask);
    int get_object_prefix(librados::IoCtx *ioctx, const std::string &oid,
			  std::string *object_prefix);
    int get_size(librados::IoCtx *ioctx, const std::string &oid,
		 snapid_t snap_id, uint64_t *size, uint8_t *order);
    int set_size(librados::IoCtx *ioctx, const std::string &oid,
		 uint64_t size);
    void set_size(librados::ObjectWriteOperation *op, uint64_t size);
    int get_parent(librados::IoCtx *ioctx, const std::string &oid,
		   snapid_t snap_id, parent_spec *pspec,
		   uint64_t *parent_overlap);
    int set_parent(librados::IoCtx *ioctx, const std::string &oid,
		   parent_spec pspec, uint64_t parent_overlap);
    int get_flags(librados::IoCtx *ioctx, const std::string &oid,
		  uint64_t *flags, const std::vector<snapid_t> &snap_ids,
		  vector<uint64_t> *snap_flags);
    void set_flags(librados::ObjectWriteOperation *op, snapid_t snap_id,
                   uint64_t flags, uint64_t mask);
    int remove_parent(librados::IoCtx *ioctx, const std::string &oid);
    void remove_parent(librados::ObjectWriteOperation *op);
    int add_child(librados::IoCtx *ioctx, const std::string &oid,
		  parent_spec pspec, const std::string &c_imageid);
    void remove_child(librados::ObjectWriteOperation *op,
		      parent_spec pspec, const std::string &c_imageid);
    int remove_child(librados::IoCtx *ioctx, const std::string &oid,
		     parent_spec pspec, const std::string &c_imageid);
    int get_children(librados::IoCtx *ioctx, const std::string &oid,
                      parent_spec pspec, set<string>& children);
    void get_children(librados::IoCtx *ioctx, const std::string &oid,
                      const parent_spec &pspec, std::set<string> *children,
                      Context *on_finish);
    void snapshot_add(librados::ObjectWriteOperation *op, snapid_t snap_id,
		      const std::string &snap_name);
    void snapshot_remove(librados::ObjectWriteOperation *op, snapid_t snap_id);
    void snapshot_rename(librados::ObjectWriteOperation *op,
			snapid_t src_snap_id,
			const std::string &dst_name);
    int get_snapcontext(librados::IoCtx *ioctx, const std::string &oid,
			::SnapContext *snapc);
    int snapshot_list(librados::IoCtx *ioctx, const std::string &oid,
		      const std::vector<snapid_t> &ids,
		      std::vector<string> *names,
		      std::vector<uint64_t> *sizes,
		      std::vector<parent_info> *parents,
		      std::vector<uint8_t> *protection_statuses);
    int copyup(librados::IoCtx *ioctx, const std::string &oid,
	       bufferlist data);
    int get_protection_status(librados::IoCtx *ioctx, const std::string &oid,
			      snapid_t snap_id, uint8_t *protection_status);
    int set_protection_status(librados::IoCtx *ioctx, const std::string &oid,
			      snapid_t snap_id, uint8_t protection_status);
    void set_protection_status(librados::ObjectWriteOperation *op,
                               snapid_t snap_id, uint8_t protection_status);
    int get_stripe_unit_count(librados::IoCtx *ioctx, const std::string &oid,
			      uint64_t *stripe_unit, uint64_t *stripe_count);
    int set_stripe_unit_count(librados::IoCtx *ioctx, const std::string &oid,
			      uint64_t stripe_unit, uint64_t stripe_count);
    int metadata_list(librados::IoCtx *ioctx, const std::string &oid,
                      const std::string &start, uint64_t max_return,
                      map<string, bufferlist> *pairs);
    int metadata_set(librados::IoCtx *ioctx, const std::string &oid,
                     const map<std::string, bufferlist> &data);
    int metadata_remove(librados::IoCtx *ioctx, const std::string &oid,
                        const std::string &key);
    int metadata_get(librados::IoCtx *ioctx, const std::string &oid,
                     const std::string &key, string *v);

    // operations on rbd_id objects
    int get_id(librados::IoCtx *ioctx, const std::string &oid, std::string *id);
    int set_id(librados::IoCtx *ioctx, const std::string &oid, std::string id);

    // operations on rbd_directory objects
    int dir_get_id(librados::IoCtx *ioctx, const std::string &oid,
		   const std::string &name, std::string *id);
    int dir_get_name(librados::IoCtx *ioctx, const std::string &oid,
		     const std::string &id, std::string *name);
    int dir_list(librados::IoCtx *ioctx, const std::string &oid,
		 const std::string &start, uint64_t max_return,
		 map<string, string> *images);
    int dir_add_image(librados::IoCtx *ioctx, const std::string &oid,
		      const std::string &name, const std::string &id);
    int dir_remove_image(librados::IoCtx *ioctx, const std::string &oid,
			 const std::string &name, const std::string &id);
    // atomic remove and add
    void dir_rename_image(librados::ObjectWriteOperation *op,
			  const std::string &src, const std::string &dest,
			  const std::string &id);

    // operations on the rbd_object_map.$image_id object
    int object_map_load(librados::IoCtx *ioctx, const std::string &oid,
		        ceph::BitVector<2> *object_map);
    void object_map_load(librados::IoCtx *ioctx, const std::string &oid,
                         ceph::BitVector<2> *object_map, Context *on_finish);
    void object_map_save(librados::ObjectWriteOperation *rados_op,
                         const ceph::BitVector<2> &object_map);
    void object_map_resize(librados::ObjectWriteOperation *rados_op,
			   uint64_t object_count, uint8_t default_state);
    void object_map_update(librados::ObjectWriteOperation *rados_op,
			   uint64_t start_object_no, uint64_t end_object_no,
			   uint8_t new_object_state,
			   const boost::optional<uint8_t> &current_object_state);
    void object_map_snap_add(librados::ObjectWriteOperation *rados_op);
    void object_map_snap_remove(librados::ObjectWriteOperation *rados_op,
                                const ceph::BitVector<2> &object_map);

    // class operations on the old format, kept for
    // backwards compatability
    void old_snapshot_add(librados::ObjectWriteOperation *rados_op,
                          snapid_t snap_id, const std::string &snap_name);
    void old_snapshot_remove(librados::ObjectWriteOperation *rados_op,
			    const std::string &snap_name);
    void old_snapshot_rename(librados::ObjectWriteOperation *rados_op,
			     snapid_t src_snap_id, const std::string &dst_name);
    int old_snapshot_list(librados::IoCtx *ioctx, const std::string &oid,
			  std::vector<string> *names,
			  std::vector<uint64_t> *sizes,
			  ::SnapContext *snapc);

    // operations on the rbd_pool_settings object
    int mirror_is_enabled(librados::IoCtx *ioctx, bool *enabled);
    int mirror_set_enabled(librados::IoCtx *ioctx, bool enabled);
    int mirror_peer_list(librados::IoCtx *ioctx,
                         std::vector<cls::rbd::MirrorPeer> *peers);
    int mirror_peer_add(librados::IoCtx *ioctx, const std::string &cluster_uuid,
                        const std::string &cluster_name,
                        const std::string &client_name);
    int mirror_peer_remove(librados::IoCtx *ioctx,
                           const std::string &cluster_uuid);
    int mirror_peer_set_client(librados::IoCtx *ioctx,
                               const std::string &cluster_uuid,
                               const std::string &client_name);
    int mirror_peer_set_cluster(librados::IoCtx *ioctx,
                                const std::string &cluster_uuid,
                                const std::string &cluster_name);

  } // namespace cls_client
} // namespace librbd
#endif // CEPH_LIBRBD_CLS_RBD_CLIENT_H
