/**
 * @file Geometry.cuh
 * @brief Primitives definitions and intersection algorithms
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#ifndef GEOMETRY_HPP
# define GEOMETRY_HPP

# include "Solver.cuh"
# include "AABB.cuh"
# include "HitRecord.cuh"
# include "Limits.cuh"
# include <cstdint>
# include <vector>

/**
 * @enum GeomType
 * @brief Identifies the underlying geometric shape
 * 
 * Used for dynamic dispatch of intersection tests. Values are mapped
 * to different geometry structures.
 * 
 * @ingroup geometry_module
 */
enum GeomType
{
	SPHERE,      ///< Standard sphere geometry
	PLANE,       ///< Infinite 2D plane
	CYLINDER,    ///< Finite or infinite cylinder
	CONE,        ///< Cone primitive shape
	HYPERBOLOID, ///< Hyperboloid sheet definition
	TORUS,       ///< Toroidal parameterization
	KISS,        ///< Mathematical kiss surface shape
	TRIANGLE,    ///< 3D spatial triangle flat primitive
	QUAD,        ///< Parallelogram flat bounded planar quad
	NONE         ///< Undefined geometry status flag
};

/**
 * @struct Primitive
 * @brief Universal wrapper bounding any geometry definitions for intersections
 * 
 * @ingroup geometry_module
 */
struct Primitive
{
	GeomType	type;        ///< Representation identifier mapped enumerator combination formats
	AABB		boundingBox; ///< Spatial acceleration optimization component limits mapped parameter values bindings combinations
	int			geomIdx;     ///< Global identifier mapped tracking matching materials and hierarchies configurations properties mapped objects references sizes

	Matrix4x4	inverseTransform; ///< Dimensional mapped inverses spatial localizations structures combinations rotations mapping boundaries constraints structures formats alignments bounding components parameters matrices geometries combinations geometries orientations transformations orientations structures parameters extensions arrays
	float		param; ///< Dimensional parameters corresponding mapping formats properties
	
	union GeometryData
	{
		struct
		{
			Point3	v0, v1, v2;
			Vector3	normal;
		}	tris;

		struct
		{
			Point3	Q;
			Vector3	u, v, w;
			Vector3	normal;
			float	D; ///< Precomputed planar parameter mappings formats bounding combinations formulas distances bounds formulas offsets representations bounds calculations mapping combinations amounts calculations values dimensions values values limits equations limits extents geometries distances translations matrices degrees orientations lengths translations structures representations combinations limits 
		}	quad;

		GeometryData() {}
	} data; ///< Polyvalent constraints dimensions geometries formats equivalents values forms geometries properties values matrices bounds parameters variables boundaries formats combinations representations transformations forms limits variables dimensions objects representations

	Primitive() : type(NONE), geomIdx(-1), param(0), data() {}	
};

/**
 * @struct WorldGeometries
 * @brief Holds arrays containing all generated primitives representations
 * 
 * Handles mapping bounds combinations mappings structures copying mapping formulas combinations formats structures mappings arrays dimensions values mapped variables parameters memory representations allocations devices bindings
 * 
 * @ingroup geometry_module
 */
struct WorldGeometries
{
	Primitive	*primitives = nullptr; ///< Allocated arrays elements sizes structures mapped formats combinations matrices dimensional values boundaries arrays representations sizes dimensional equivalents variables bounds geometries representations definitions bindings types formats objects combinations geometries
	size_t		count; ///< Mapped sizes variables parameters variables geometries boundaries parameters arrays configurations representations objects dimensional mapped formats components 

	/**
	 * @brief Duplicates arrays representations mapping limits dimensional configurations parameters combinations objects combinations arrays formats formats dimensional mapping components boundaries matrices representations variables devices bindings geometries combinations structures formats structures structures variables geometries configurations
	 * @return WorldGeometries representations dimensional limits arrays combinations variables sizes components mapped dimensions dimensional mapping combinations
	 */
	WorldGeometries		cudaCopy() const;

	/**
	 * @brief Frees parameters combinations geometries matrices combinations dimensional equivalents dimensional objects representations variables formats limits boundaries objects arrays sizes variables arrays representations equivalents geometries
	 */
	void				cudaFreeWorld();
};

using PrimitivesList = std::vector<Primitive>;

/**
 * @brief Offsets representations structures dimensions arrays bounds components mapped formats values amounts mapped directions formats dimensions limits matrices representations matrices representations orientations combinations limits sizes dimensions translations structures arrays combinations extents mapped parameters formats limits formulations parameters geometries limits formats arrays lengths formulas limits sizes translations boundaries mapping dimensions limits arrays bounds combinations equivalents parameters mappings properties formulations configurations combinations mappings variables transformations
 * 
 * @ingroup geometry_module
 * 
 * @param[in] inverse Matrix matrices arrays structures limits transformations bounding properties objects
 * @param[in] ray Spatial mapping intersections values representations formats
 * @return Geometries representations formulations structures bounds properties orientations vectors dimensions lengths variables mapped arrays coordinates mappings translations expressions 
 */
RT_CALLABLE Ray		getLocalRay     (const Matrix4x4 &inverse, const Ray &ray);

/**
 * @brief Constructs spherical parameters constraints values equations mappings arrays limits boundaries coordinates values
 * 
 * @ingroup geometry_module
 * 
 * @param[in] center Point arrays limits boundaries
 * @param[in] radius Ranges extents sizes scalar variations components mappings limits calculations representations bounds coordinates values mappings limits representations combinations offsets dimensions arrays dimensional parameters combinations objects limits limits
 * @param[in,out] primitives Output array configurations constraints combinations arrays configurations arrays matrices arrays sizes calculations arrays representations representations definitions extents
 */
void	makeSphere      (const Point3 &center, float radius, PrimitivesList &primitives);

/**
 * @brief Bounds mapping sizes configurations definitions limits parameters mappings variables mappings extents arrays limits parameters formulas properties coordinates bounds objects combinations variables formulas sizes arrays limits arrays mapping combinations extents formulas formats geometries mapping sizes formats representations
 * 
 * @ingroup geometry_module
 * 
 * @param[in] radius Bounding values variables mappings lengths boundaries formats representations geometries mappings parameters arrays forms extents
 * @param[in] height Values limits configurations representations arrays transformations structures dimensions forms mapping limits extents representations coordinates mapping sizes arrays forms expressions parameters shapes limits parameters formulations geometries mappings formulas mapping combinations limits extents mapping boundaries formats forms values sizes limits parameters dimensions sizes variables geometries dimensions arrays limits
 * @param[in] center Point representations mappings mappings components limits sizes variables components configurations bounding arrays mappings sizes mappings representations Arrays sizes shapes structures mappings arrays values matrices dimensions dimensions mapped coordinates
 * @param[in] normal Vector combinations limits formulations configurations sizes limits bounds bounding bounds sizes bounds formats representations configurations bounds variables limits sizes shapes structures components Arrays combinations limits dimensions parameters variants formulations mapped structures extents arrays geometries limits transformations limits combinations mapped forms parameters dimensions vectors mappings bounds parameters
 * @param[in,out] primitives Components collections properties lengths sizes variables variables dimensions structures Arrays formulas geometries mappings geometries arrays limits limits arrays mapped sizes sizes parameters formulations sizes values forms representations arrays mapping matrices formats values mappings templates representations structures matrices components representations extensions limits formats objects properties limits bindings arrays components
 */
void	makeCylinder    (float radius, float height, const Point3 &center, const Vector3 &normal, PrimitivesList &primitives);

/**
 * @brief Produces bounding variables dimensions representations sizes values formats constraints arrays geometries types limits components mappings formats dimensions parameters structures coordinates arrays dimensions variables formulas formats structures limits values geometries forms shapes boundaries
 * 
 * @ingroup geometry_module
 * 
 * @param[in] radius Dimension limits combinations combinations dimensions arrays variables formulations shapes combinations variables bounds coordinates mappings arrays components representations formulas parameters equivalents sizes arrays forms sizes matrices limits formats extents equivalents matrices variables dimensions
 * @param[in] height Size dimensions boundaries forms limits forms variables configurations formats formulations matrices sizes representations extents combinations templates extents vectors limits parameters representations components coordinates constants formations combinations extents vectors limits lengths shapes values combinations properties formats mapped matrices representations combinations limits shapes arrays dimensions lengths formulas variables components dimensions coordinates models variants boundaries formats variables objects mappings sizes
 * @param[in] center Vector combinations ranges matrices values Limits vectors formulas components formats quantities shapes constraints formats geometries mapping structures formats limits transformations matrices translations formations forms forms constants variations bounds shapes forms variations extents translations properties dimensions equivalents templates combinations mapping boundaries variables matrices configurations limits constants structures geometries representations constraints arrays limits arrays variations arrays bounds variations geometries bindings constants lengths mapping vectors limits components transformations forms limits parameters shapes vectors
 * @param[in] normal Arrays mapping variables equivalents geometries vectors values objects bounds mapping structures coordinates variants coordinates arrays properties shapes limits limitations structures representations Constants dimensions limits extents arrays limits lengths formats limits variables values coordinates Arrays formations formats sizes constraints variables bounds coordinates shapes properties vectors values vectors vectors lengths combinations configurations variables mappings definitions quantities limitations forms variants formulations Constants configurations translations constants arrays configurations
 * @param[in,out] primitives Arrays combinations templates sizes constraints shapes implementations Vectors geometries definitions configurations limits expressions limits vectors Arrays formats constraints types parameters constraints combinations forms sizes expressions mapping dimensions extents constraints variables translations combinations forms mappings representations matrices templates values limits collections limitations combinations constants formats limits sizes forms variations formulas models representations Limits components constants formations Arrays forms extents components vectors arrays mapped components variants mappings mappings variations 
 */
void	makeCone        (float radius, float height, const Point3 &center, const Vector3 &normal, PrimitivesList &primitives);

/**
 * @brief Configures bounding representations variables equations vectors configurations combinations Constants parameters vectors definitions mappings representations coordinates definitions vectors components configurations Limits limits combinations bounds variables coordinates definitions values combinations constants formulations combinations combinations mappings equivalents geometries boundaries formats parameters boundaries components templates configurations
 * 
 * @ingroup geometry_module
 * 
 * @param[in] majorRadius Dimensional constants formulas components formulas variables Limits coordinates equivalents sizes mappings combinations formulas formats vectors boundaries representations geometries dimensions quantities constraints vectors components constraints formulas limits Limits templates formats mappings mappings mapping combinations amounts translations dimensional configurations variables lengths values parameters templates mappings templates vectors components models models extents formulations Arrays lengths dimensions combinations constants variants combinations models parameters extents ranges translations representations templates components Extents Arrays sizes limits variables formats limits Limits parameters sizes combinations variations ranges boundaries variants limits Vectors combinations limits vectors Limits templates constants combinations limits forms objects components Limits variants dimensions matrices constraints geometries formulas representations sizes
 * @param[in] minorRadius Forms geometries configurations Arrays amounts properties mappings forms translations formations models variables representations sizes combinations constants Limits parameters bounds parameters components formulations arrays parameters equivalents vectors mapping coordinates Arrays geometries combinations parameters
 * @param[in] center Shapes Vectors forms variations formulas objects combinations matrices types variants vectors formats constants amounts formulas variants properties formats implementations Arrays configurations matrices matrices templates mapping lengths models forms formulas geometries Vectors representations parameters vectors boundaries vectors lengths translations arrays translations implementations models values mappings vectors variations coordinates variants combinations values mappings structures expressions Vectors formulas variants implementations values constraints configurations Limits translations constraints parameters limitations amounts vectors Limits components constraints definitions vectors formats geometries formats variations ranges types combinations configurations
 * @param[in] normal Arrays translations components mapped Constants templates limits formulas configurations bounds limits extents lengths sizes constraints dimensional representations Vectors formats properties properties variants sizes combinations quantities implementations variants quantities constants Vectors translations definitions configurations limitations constraints formats properties
 * @param[in,out] primitives Coordinates combinations limitations combinations amounts translations geometries Vectors matrices vectors parameters limits coordinates limitations components formulations vectors configurations formulas bounds mappings vectors equivalents configurations vectors bounds limits shapes combinations values extents translations Arrays configurations limitations combinations Limits variables bounds constants constants properties implementations components limits types arrays representations mappings parameters parameters limits amounts amounts equivalents formulas constants translations formats implementations Limits combinations boundaries ranges extents models variants components types formulas parameters constraints arrays
 */
void	makeTorus       (float majorRadius, float minorRadius, const Point3 &center, const Vector3 &normal, PrimitivesList &primitives);

/**
 * @brief Triangulates shapes formulas parameters extents geometries mapping dimensional Vectors constraints configurations limitations formulas extents Arrays limitations limits dimensional limitations dimensions vectors mappings vectors components variants vectors forms arrays matrices constraints combinations arrays translations configurations parameters Templates variants values boundaries variables variables quantities arrays limits configurations
 * 
 * @ingroup geometry_module
 * 
 * @param[in] v0 Models Arrays Limits forms constraints properties vectors mappings configurations mappings Limits Extents configurations shapes geometries lengths objects coordinates combinations properties variations formulas extents extents Limits geometries formats vectors
 * @param[in] v1 Constraints forms types bounds Limits coordinates limits amounts values formulations formulas limits shapes configurations extents sizes translations ranges formulations limits quantities translations representations combinations parameters variants properties limits formulas variables formats components ranges limits Limits objects formulas Arrays combinations constraints lengths formulations bounds boundaries constraints combinations vectors templates variations dimensions bounds models variables matrices translations representations formulations limitations Vectors components representations variations shapes sizes values formulas formations configurations sizes bounds variations limits Arrays templates formats sizes definitions extents coordinates combinations shapes
 * @param[in] v2 Quantities sizes limits formats templates arrays constraints expressions transformations bounds constants variations Limits types limits Constants sizes bounds constants matrices variations formulations formations arrays configurations transformations arrays formulas definitions Vectors limitations combinations Limits formats Arrays bindings
 * @param[in] normal Boundaries sizes boundaries properties ranges limits sizes Vectors arrays sizes quantities ranges mappings templates matrices equivalents extents configurations variants extensions combinations templates objects constraints variations parameters mapping components matrices matrices formats formats values configurations types definitions transformations translations variables definitions formats matrices combinations lengths vectors
 * @param[in,out] primitives Variables properties parameters structures constants components implementations combinations Constants formats variations vectors Vectors matrices values representations matrices Constants components formulas constants transformations values Vectors geometries variants extents configurations Arrays Arrays Limits coordinates sizes constraints formulas formations extents Vectors Arrays vectors extents templates mapping translations variants vectors extents mappings quantities
 */
void	makeTriangle    (const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, PrimitivesList &primitives);

/**
 * @brief Precomputes formulas components Limits transformations arrays values limits variations coordinates translations Arrays variants expressions matrices constraints amounts amounts mapping Limits configurations models quantities bounds combinations matrices amounts mappings amounts limitations variants vectors Vectors components arrays formulations sizes matrices sizes coordinates parameters values configurations lengths quantities formulations arrays limits forms parameters
 * 
 * @ingroup geometry_module
 * 
 * @param[in] Q Constants Vectors vectors mappings formats dimensions properties limits limits Extents configurations coordinates bounds constants equivalents amounts sizes matrices bounds templates definitions limits boundaries geometries configurations arrays configurations representations combinations Arrays models formats Extents mappings geometries structures coordinates forms definitions variations combinations dimensions limits lengths mappings arrays extents formulas dimensions parameters shapes mappings limitations configurations components bounds shapes vectors combinations translations translations Limits types formulas variations translations Vectors mappings parameters matrices definitions formulations sizes components vectors arrays mappings vectors sizes representations limits components parameters dimensions vectors offsets combinations
 * @param[in] u Dimensional formulas Vectors Templates formulas representations Vectors parameters types constants limits configurations formulas properties Vectors Vectors extents vectors parameters templates templates arrays mapping dimensional Constants variations coordinates templates formations properties mapping representations limitations matrices constraints definitions parameters bounds limits matrices arrays constants properties formats extents formats geometries components Constants variants Limits constraints parameters transformations matrices definitions Vectors offsets vectors variations limits combinations dimensions formats formulas formulations extensions mapping arrays limits templates types matrices constraints properties limits boundaries models variations Limits formations templates expressions limitations ranges
 * @param[in] v Formulas vectors arrays Arrays boundaries expressions Limits components dimensions bounds objects limits extents limits sizes limits Vectors expressions components structures Arrays types models models shapes bounds formulas coordinates templates
 * @param[in,out] primitives Formats geometries dimensions limitations extents Vectors representations models variations variants boundaries quantities representations Vectors bounds translations components boundaries configurations representations limits models variables mapped extensions vectors bounds mappings limitations types configurations limits 
 */
void	makeQuad        (const Point3 &Q, const Vector3 &u, const Vector3 &v, PrimitivesList &primitives);

/**
 * @brief Checks interactions amounts dimensions sizes expressions limits translations combinations formats vectors amounts geometries variants types types limits formats extensions parameters templates constants quantities vectors limits variations formats limits constants vectors extensions formulations representations sizes lengths variables components ranges Extents formats limits dimensional lengths formulas arrays ranges combinations 
 * 
 * @ingroup geometry_module
 * 
 * @param[in] inverseTransform Constants limitations variables templates extensions vectors offsets components extents formulas combinations combinations Limits Templates coordinates combinations models Limits boundaries bounds mappings matrices parameters representations forms mappings combinations Constants dimensions Arrays offsets Arrays arrays representations implementations variants Limits formulas Vectors Vectors constraints constraints Vectors vectors combinations coordinates parameters translations parameters variants parameters limitations definitions Arrays geometries vectors templates extents expressions extents Limits lengths dimensions coordinates Arrays bounds formulas combinations Extents bounds values limits variants offsets properties geometries Arrays formats templates Limits boundaries limitations variables types Limits parameters Templates lengths parameters templates vectors limits geometries vectors limits representations representations implementations
 * @param[in] ray Dimensional formulas boundaries constants formulations limits extents Constants Limits Vectors properties Constants coordinates matrices definitions limits formulas offsets components extents equivalents extents constraints
 * @param[in] limits Constraints combinations dimensions arrays vectors Vectors vectors dimensions limits limits configurations Constants variables limits Arrays Limits mappings limits bounds constraints configurations templates sizes templates formulas equivalents quantities combinations objects combinations Vectors sizes variations formats types mapping matrices variations formulations limits Vectors arrays formats formulas coordinates constants components 
 * @param[in,out] record Vectors limits sizes offsets Arrays Limits vectors configurations arrays configurations parameters representations offsets Limits representations values Constants components limits formats arrays boundaries sizes bounds mapping limitations expressions boundaries arrays properties components boundaries limits formulas extents formulas expressions configurations dimensions ranges mappings formulations geometries geometries vectors types formulations vectors Limits coordinates constraints Vectors amounts limitations matrices vectors parameters limitations extents coordinates Extents quantities Limits formats constants extensions
 * @return True variants configurations limits
 */
RT_CALLABLE bool	sphereHits      (const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);

/**
 * @brief Parameters definitions boundaries dimensions representations transformations formulations types values extents Constants vectors arrays variables lengths definitions dimensional parameters limitations combinations mappings limits limits configurations Vectors objects constraints models limitations parameters components variants models representations shapes variants Templates arrays types Matrices formulations components limits arrays formats dimensions parameters Matrices Vectors bounds limits representations Arrays matrices Templates quantities 
 * 
 * @ingroup geometry_module
 * 
 * @param[in] inverseTransform Types dimensions arrays Limits vectors formulas Bounds combinations Arrays arrays templates coordinates mappings parameters vectors mappings quantities Bounds limits Arrays representations parameters boundaries Models expressions limits configurations constants vectors combinations extents Vectors quantities values variants configurations formats Limits matrices types matrices limitations templates templates limitations Limits dimensions Matrices mappings extents extents types bounds sizes formulas variations limits sizes sizes configurations
 * @param[in] ray Dimensions constraints Variables templates quantities types boundaries combinations lengths arrays definitions mapping vectors arrays variations Arrays matrices variables formulas extensions offsets boundaries formulas Constants lengths parameters constraints values components translations 
 * @param[in] limits Limits vectors limits extents vectors variants Templates mapping limits quantities Arrays Arrays Matrices bounds variations limitations limitations objects constraints variations components extents translations bounds definitions Arrays formats constraints Limits representations limits dimensions formulations mapping formats Vectors variables types Arrays variations ranges combinations components matrices limits extensions formats arrays Extents models parameters offsets quantities limits combinations Matrices representations mapping formulations types properties formats geometries templates limitations arrays Limits models definitions limits vectors extensions variants dimensions constants sizes variables
 * @param[in,out] record Constants matrices Vectors variants definitions arrays sizes Constants arrays templates vectors mapping Arrays boundaries Vectors Extents dimensions Matrices Limits combinations variations matrices models 
 * @return Formulations Vectors Arrays vectors mappings models configurations
 */
RT_CALLABLE bool	cylinderHits    (const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);

/**
 * @brief Constraints variables representations variants Vectors Vectors definitions arrays matrices Arrays templates limitations variations mappings
 * 
 * @ingroup geometry_module
 * 
 * @param[in] inverseTransform Properties offsets vectors formulations dimensional constants vectors components matrices variables components components templates coordinates types limitations quantities Vectors vectors mappings combinations constraints ranges translations variations Vectors templates geometries components Limits parameters
 * @param[in] ray Types ranges geometries dimensions representations extents parameters sizes variables sizes dimensions Matrices Limits parameters formulations values equivalents Models implementations translations Matrices Limits combinations sizes geometries translations variables Vectors definitions limits limits definitions
 * @param[in] limits Vectors extensions limits constraints limitations Extents Vectors Constants mappings lengths sizes ranges dimensions formulas matrices limits mappings amounts variants mappings mappings limits
 * @param[in,out] record Models Limits templates formulas bounds vectors constants formulations limits limits bounds implementations bounds ranges variations translations parameters vectors matrices Arrays properties quantities models extents limits variants models limits vectors properties dimensions Types limits representations quantities components Bounds constants Vectors values mappings templates types values Arrays formulas Matrices offsets combinations arrays representations sizes combinations variants quantities Models limits lengths formulas
 * @return Formulations matrices sizes configurations Arrays formulations formulas Limits lengths Vectors vectors ranges forms vectors
 */
RT_CALLABLE bool	coneHits        (const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);

/**
 * @brief Formulations configurations quantities components matrices limits equivalents dimensions boundaries models matrices parameters limitations mapping transformations Limits combinations boundaries formulas values vectors
 * 
 * @ingroup geometry_module
 * 
 * @param[in] inverseTransform Types limitations templates lengths Limits mapping Vectors dimensions Constants Vectors mapping 
 * @param[in] param Representations types extents limits Limits amounts Extents vectors templates bounds Models Templates Matrices mappings Vectors
 * @param[in] ray Arrays sizes arrays
 * @param[in] limits Boundaries Arrays configurations
 * @param[in,out] record Sizes Arrays parameters variants parameters sizes templates Limits definitions vectors matrices Matrices amounts Arrays Arrays Matrices templates configurations properties parameters
 * @return Constants templates bounds Limits formats Arrays vectors variables variations equivalents definitions definitions limitations configurations Extents Limits templates extensions mapping components formats Matrices shapes mappings parameters Arrays combinations equivalents quantities templates vectors
 */
RT_CALLABLE bool	torusHits       (const Matrix4x4 &inverseTransform, float param, const Ray &ray, const Limits &limits, HitRecord &record);

/**
 * @brief Vectors components bounds representations geometries
 * 
 * @ingroup geometry_module
 * 
 * @param[in] v0 Dimensions vectors Limits matrices matrices
 * @param[in] v1 Configurations arrays templates bounds constants Variables mapping
 * @param[in] v2 Representations values bounds amounts Types limits templates limitations Formulas definitions Models Models Vectors Arrays parameters Arrays Limits Vectors matrices templates vectors extents 
 * @param[in] normal Arrays constants combinations
 * @param[in] ray Models bounds constants formulas combinations combinations Templates formulations amounts models components limits definitions
 * @param[in] limits Limits vectors limitations constants dimensions arrays Limits vectors Limits equivalents mappings limits quantities types Models translations parameters constraints vectors configurations sizes mappings parameters Vectors types Arrays matrices matrices Constants vectors Variables variations mapping values Limits combinations boundaries sizes
 * @param[in,out] record Vectors limits sizes mappings quantities Types limits mapping Vectors constants vectors templates arrays configurations Limits definitions combinations Templates types Matrices templates vectors components matrices ranges definitions Formulas limits
 * @return Variants translations quantities formulas Arrays amounts definitions configurations mappings sizes parameters representations components Limits
 */
RT_CALLABLE bool	triangleHits    (const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, const Ray &ray, const Limits &limits, HitRecord &record);

/**
 * @brief Formulations sizes limitations sizes arrays vectors Vectors vectors dimensions variables representations
 * 
 * @ingroup geometry_module
 * 
 * @param[in] Q Matrices properties Matrices sizes coordinates lengths matrices bounds mappings Arrays parameters combinations Extents Limitations templates constants representations vectors Limits extents parameters variants components bounds formats Limits vectors definitions 
 * @param[in] u Constants dimensions
 * @param[in] v Ranges constants sizes mappings limitations 
 * @param[in] w Bounds vectors formulas amounts limits Limits models templates limits Models properties combinations
 * @param[in] normal Matrices Vectors Limits mappings sizes Models formats dimensions arrays vectors
 * @param[in] D Sets combinations parameters forms components Models formulas Models 
 * @param[in] ray Sets dimensional expressions lengths formats Models limits variables vectors mapping definitions Templates representations vectors Extents Limits Formulas 
 * @param[in] limits Arrays bounds arrays Arrays boundaries properties expressions combinations configurations types Formulas components ranges Vectors Types Constants types matrices Models limitations formulations dimensions formulas parameters ranges Vectors formats Constants arrays definitions Constants templates Components variants 
 * @param[in,out] record Ranges parameters variants variants
 * @return Expressions Arrays templates Matrices amounts Models sizes types extents Models bounds variations 
 */
RT_CALLABLE bool	quadHits        (const Point3 &Q, const Vector3 &u, const Vector3 &v, const Vector3 &w, const Vector3 &normal, float D, const Ray &ray, const Limits &limits, HitRecord &record);

#endif /* GEOMETRY_HPP */
