#ifndef BLENDER_DEFINITIONS_H
#define BLENDER_DEFINITIONS_H 

#define DNA_DEPRECATED

struct ID;
struct AnimData;
struct BVHCache;
struct Ipo;
struct Key;
struct LinkNode;
struct MCol;
struct MEdge;
struct MLoopCol;
struct MLoopTri;
struct MLoopUV;
struct MPropCol;
struct Material;
struct Multires;
struct SubdivCCG;
struct MTFace;
struct MDeformVert;
struct BMEditMesh;
struct Mesh_Runtime;
struct MSelect;
struct TFace;
struct CustomData;
struct IDProperty;
struct IDOverrideLibrary;


struct ID {
	void* next, * prev;
	struct ID* newid;
	struct Library* lib;
	/** MAX_ID_NAME. */
	char name[66];
	/**
	 * LIB_... flags report on status of the data-block this ID belongs to
	 * (persistent, saved to and read from .blend).
	 */
	short flag;
	/**
	 * LIB_TAG_... tags (runtime only, cleared at read time).
	 */
	int tag;
	int us;
	int icon_id;
	int recalc;
	/**
	 * Used by undo code. recalc_after_undo_push contains the changes between the
	 * last undo push and the current state. This is accumulated as IDs are tagged
	 * for update in the depsgraph, and only cleared on undo push.
	 *
	 * recalc_up_to_undo_push is saved to undo memory, and is the value of
	 * recalc_after_undo_push at the time of the undo push. This means it can be
	 * used to find the changes between undo states.
	 */
	int recalc_up_to_undo_push;
	int recalc_after_undo_push;

	/**
	 * A session-wide unique identifier for a given ID, that remain the same across potential
	 * re-allocations (e.g. due to undo/redo steps).
	 */
	unsigned int session_uuid;

	IDProperty* properties;

	/** Reference linked ID which this one overrides. */
	IDOverrideLibrary* override_library;

	/**
	 * Only set for data-blocks which are coming from copy-on-write, points to
	 * the original version of it.
	 */
	struct ID* orig_id;

	void* py_instance;
};

struct CustomDataLayer;
struct BLI_mempool;
struct CustomDataExternal;

struct CustomData {
	/** CustomDataLayers, ordered by type. */
	CustomDataLayer* layers;
	/**
	 * runtime only! - maps types to indices of first layer of that type,
	 * MUST be >= CD_NUMTYPES, but we cant use a define here.
	 * Correct size is ensured in CustomData_update_typemap assert().
	 */
	int typemap[50];
	char _pad[4];
	/** Number of layers, size of layers array. */
	int totlayer, maxlayer;
	/** In editmode, total size of all data layers. */
	int totsize;
	/** (BMesh Only): Memory pool for allocation of blocks. */
	BLI_mempool* pool;
	/** External file storing customdata layers. */
	CustomDataExternal* external;
};

struct EditMeshData;
struct BVHCache;
struct ShrinkwrapBoundaryData;
struct MLoopTri_Store {
	/* WARNING! swapping between array (ready-to-be-used data) and array_wip
	 * (where data is actually computed)
	 * shall always be protected by same lock as one used for looptris computing. */
	struct MLoopTri* array, * array_wip;
	int len;
	int len_alloc;
};

struct CustomData_MeshMasks {
	uint64_t vmask;
	uint64_t emask;
	uint64_t fmask;
	uint64_t pmask;
	uint64_t lmask;
};
struct Mesh_blender;
struct Mesh_Runtime {
	/* Evaluated mesh for objects which do not have effective modifiers.
	 * This mesh is used as a result of modifier stack evaluation.
	 * Since modifier stack evaluation is threaded on object level we need some synchronization. */
	Mesh_blender* mesh_eval;
	void* eval_mutex;

	EditMeshData* edit_data;
	void* batch_cache;

	SubdivCCG* subdiv_ccg;
	void* _pad1;
	int subdiv_ccg_tot_level;
	char _pad2[4];

	int64_t cd_dirty_vert;
	int64_t cd_dirty_edge;
	int64_t cd_dirty_loop;
	int64_t cd_dirty_poly;

	MLoopTri_Store looptris;

	/** `BVHCache` defined in 'BKE_bvhutil.c' */
	BVHCache* bvh_cache;

	/** Non-manifold boundary data for Shrinkwrap Target Project. */
	ShrinkwrapBoundaryData* shrinkwrap_data;

	/** Set by modifier stack if only deformed from original. */
	char deformed_only;
	/**
	 * Copied from edit-mesh (hint, draw with editmesh data).
	 * In the future we may leave the mesh-data empty
	 * since its not needed if we can use edit-mesh data. */
	char is_original;

	/** #eMeshWrapperType and others. */
	char wrapper_type;
	/**
	 * A type mask from wrapper_type,
	 * in case there are differences in finalizing logic between types.
	 */
	char wrapper_type_finalize;

	char _pad[4];

	/** Needed in case we need to lazily initialize the mesh. */
	CustomData_MeshMasks cd_mask_extra;

};

struct MFace {
	unsigned int v1, v2, v3, v4;
	short mat_nr;
	/** We keep edcode, for conversion to edges draw flags in old files. */
	char edcode, flag;
};
struct MLoop {
	/** Vertex index. */
	unsigned int v;
	/**
	 * Edge index.
	 *
	 * \note The e here is because we want to move away from relying on edge hashes.
	 */
	unsigned int e;
};


struct MPoly {
	/** Offset into loop array and number of loops in the face. */
	int loopstart;
	/** Keep signed since we need to subtract when getting the previous loop. */
	int totloop;
	short mat_nr;
	char flag, _pad;
};

struct MVert {
	float co[3];
	/**
	 * Cache the normal, can always be recalculated from surrounding faces.
	 * See #CD_CUSTOMLOOPNORMAL for custom normals.
	 */
	short no[3];
	char flag, bweight;
};

struct Mesh_blender {
	ID id;
	/** Animation data (must be immediately after id for utilities to use it). */
	AnimData* adt;

	/** Old animation system, deprecated for 2.5. */
	Ipo* ipo DNA_DEPRECATED;
	Key* key;
	Material** mat;
	MSelect* mselect;

	/* BMESH ONLY */
	/*new face structures*/
	MPoly* mpoly;
	MLoop* mloop;
	MLoopUV* mloopuv;
	MLoopCol* mloopcol;
	/* END BMESH ONLY */

	/**
	 * Legacy face storage (quads & tries only),
	 * faces are now stored in #Mesh.mpoly & #Mesh.mloop arrays.
	 *
	 * \note This would be marked deprecated however the particles still use this at run-time
	 * for placing particles on the mesh (something which should be eventually upgraded).
	 */
	MFace* mface;
	/** Store tessellation face UV's and texture here. */
	MTFace* mtface;
	/** Deprecated, use mtface. */
	TFace* tface DNA_DEPRECATED;
	/** Array of verts. */
	MVert* mvert;
	/** Array of edges. */
	MEdge* medge;
	/** Deformgroup vertices. */
	MDeformVert* dvert;

	/* array of colors for the tessellated faces, must be number of tessellated
	 * faces * 4 in length */
	MCol* mcol;
	Mesh_blender* texcomesh;

	/* When the object is available, the preferred access method is: BKE_editmesh_from_object(ob) */
	/** Not saved in file!. */
	BMEditMesh* edit_mesh;

	CustomData vdata, edata, fdata;

	/* BMESH ONLY */
	CustomData pdata, ldata;
	/* END BMESH ONLY */

	int totvert, totedge, totface, totselect;

	/* BMESH ONLY */
	int totpoly, totloop;
	/* END BMESH ONLY */

	/* the last selected vertex/edge/face are used for the active face however
	 * this means the active face must always be selected, this is to keep track
	 * of the last selected face and is similar to the old active face flag where
	 * the face does not need to be selected, -1 is inactive */
	int act_face;

	/* texture space, copied as one block in editobject.c */
	float loc[3];
	float size[3];

	short texflag, flag;
	float smoothresh;

	/* customdata flag, for bevel-weight and crease, which are now optional */
	char cd_flag, _pad;

	char subdiv DNA_DEPRECATED, subdivr DNA_DEPRECATED;
	/** Only kept for backwards compat, not used anymore. */
	char subsurftype DNA_DEPRECATED;
	char editflag;

	short totcol;

	float remesh_voxel_size;
	float remesh_voxel_adaptivity;
	char remesh_mode;

	char _pad1[3];

	int face_sets_color_seed;
	/* Stores the initial Face Set to be rendered white. This way the overlay can be enabled by
	 * default and Face Sets can be used without affecting the color of the mesh. */
	int face_sets_color_default;

	/** Deprecated multiresolution modeling data, only keep for loading old files. */
	Multires* mr DNA_DEPRECATED;

	Mesh_Runtime runtime;
};



#endif
