/*

Arduino program to drive a servo motor indicating when Mars is above the horizon in Seattle Washington (122°20'13.2''W, 47°36'32.0''N).
The time is in Pacific Standard Time (PST).

Do NOT adjust the time for daylight savings time! Always set it for standard time.

This code is available at https://github.com/chrisspurgeon/Hannah_orrery


*/





#include "String.h"
#include "Wire.h"
#include "Servo.h"
#include <avr/pgmspace.h>


#define DS1307_I2C_ADDRESS 0x68
#define ledPin 13

int lastSecond = 0;
int pos = 0;
int UP = 170;
int DOWN = 95;

Servo myservo;

long convertedDate;
long lastEvent;
unsigned long horizonTimeLong;

int marsLength = 2326;    //    This is the number of data entries in the mars array below;

int MARS = 0;


// The data entries below capture when Mars dips below the horion and when it rises above the horizon.
// Each entry is in two consecutive parts. The first is a time stamp in this format: YYMMDDHHMM. 
// So if Mars rises on July 4th, 2012 at 9:42AM, the entries would look like this...
//                                          1207040942, 1,

PROGMEM prog_uint32_t mars[]={
1204280305, 0, 1204281319, 1, 1204290300, 0, 1204291316, 1, 1206010106, 0, 
1206011203, 1, 1206020102, 0, 1206021202, 1, 1206030059, 0, 1206031200, 1, 
1206040056, 0, 1206041159, 1, 1206050053, 0, 1206051157, 1, 1206060049, 0, 
1206061155, 1, 1206070046, 0, 1206071154, 1, 1206080043, 0, 1206081152, 1, 
1206090040, 0, 1206091151, 1, 1206100036, 0, 1206101149, 1, 1206110033, 0, 
1206111148, 1, 1206120030, 0, 1206121147, 1, 1206130027, 0, 1206131145, 1, 
1206140024, 0, 1206141144, 1, 1206150021, 0, 1206151142, 1, 1206160017, 0, 
1206161141, 1, 1206170014, 0, 1206171140, 1, 1206180011, 0, 1206181138, 1, 
1206190008, 0, 1206191137, 1, 1206200005, 0, 1206201136, 1, 1206210002, 0, 
1206211135, 1, 1206212359, 0, 1206221133, 1, 1206222355, 0, 1206231132, 1, 
1206232352, 0, 1206241131, 1, 1206242349, 0, 1206251130, 1, 1206252346, 0, 
1206261129, 1, 1206262343, 0, 1206271127, 1, 1206272340, 0, 1206281126, 1, 
1206282337, 0, 1206291125, 1, 1206292334, 0, 1206301124, 1, 1206302331, 0, 
1207011123, 1, 1207012328, 0, 1207021122, 1, 1207022325, 0, 1207031121, 1, 
1207032322, 0, 1207041120, 1, 1207042319, 0, 1207051119, 1, 1207052316, 0, 
1207061118, 1, 1207062313, 0, 1207071117, 1, 1207072310, 0, 1207081116, 1, 
1207082307, 0, 1207091115, 1, 1207092304, 0, 1207101114, 1, 1207102301, 0, 
1207111113, 1, 1207112258, 0, 1207121112, 1, 1207122255, 0, 1207131111, 1, 
1207132252, 0, 1207141110, 1, 1207142249, 0, 1207151109, 1, 1207152246, 0, 
1207161109, 1, 1207162243, 0, 1207171108, 1, 1207172240, 0, 1207181107, 1, 
1207182237, 0, 1207191106, 1, 1207192234, 0, 1207201105, 1, 1207202231, 0, 
1207211104, 1, 1207212228, 0, 1207221104, 1, 1207222225, 0, 1207231103, 1, 
1207232223, 0, 1207241102, 1, 1207242220, 0, 1207251101, 1, 1207252217, 0, 
1207261101, 1, 1207262214, 0, 1207271100, 1, 1207272211, 0, 1207281059, 1, 
1207282208, 0, 1207291058, 1, 1207292205, 0, 1207301058, 1, 1207302203, 0, 
1207311057, 1, 1207312200, 0, 1208011056, 1, 1208012157, 0, 1208021056, 1, 
1208022154, 0, 1208031055, 1, 1208032151, 0, 1208041054, 1, 1208042148, 0, 
1208051054, 1, 1208052146, 0, 1208061053, 1, 1208062143, 0, 1208071053, 1, 
1208072140, 0, 1208081052, 1, 1208082137, 0, 1208091052, 1, 1208092135, 0, 
1208101051, 1, 1208102132, 0, 1208111050, 1, 1208112129, 0, 1208121050, 1, 
1208122126, 0, 1208131049, 1, 1208132124, 0, 1208141049, 1, 1208142121, 0, 
1208151048, 1, 1208152118, 0, 1208161048, 1, 1208162115, 0, 1208171047, 1, 
1208172113, 0, 1208181047, 1, 1208182110, 0, 1208191046, 1, 1208192107, 0, 
1208201046, 1, 1208202105, 0, 1208211046, 1, 1208212102, 0, 1208221045, 1, 
1208222059, 0, 1208231045, 1, 1208232057, 0, 1208241044, 1, 1208242054, 0, 
1208251044, 1, 1208252052, 0, 1208261044, 1, 1208262049, 0, 1208271043, 1, 
1208272046, 0, 1208281043, 1, 1208282044, 0, 1208291043, 1, 1208292041, 0, 
1208301042, 1, 1208302039, 0, 1208311042, 1, 1208312036, 0, 1209011042, 1, 
1209012034, 0, 1209021041, 1, 1209022031, 0, 1209031041, 1, 1209032029, 0, 
1209041041, 1, 1209042026, 0, 1209051040, 1, 1209052024, 0, 1209061040, 1, 
1209062021, 0, 1209071040, 1, 1209072019, 0, 1209081040, 1, 1209082017, 0, 
1209091039, 1, 1209092014, 0, 1209101039, 1, 1209102012, 0, 1209111039, 1, 
1209112009, 0, 1209121039, 1, 1209122007, 0, 1209131038, 1, 1209132005, 0, 
1209141038, 1, 1209142002, 0, 1209151038, 1, 1209152000, 0, 1209161038, 1, 
1209161958, 0, 1209171038, 1, 1209171956, 0, 1209181038, 1, 1209181953, 0, 
1209191037, 1, 1209191951, 0, 1209201037, 1, 1209201949, 0, 1209211037, 1, 
1209211947, 0, 1209221037, 1, 1209221944, 0, 1209231037, 1, 1209231942, 0, 
1209241037, 1, 1209241940, 0, 1209251036, 1, 1209251938, 0, 1209261036, 1, 
1209261936, 0, 1209271036, 1, 1209271934, 0, 1209281036, 1, 1209281932, 0, 
1209291036, 1, 1209291930, 0, 1209301036, 1, 1209301928, 0, 1210011036, 1, 
1210011926, 0, 1210021036, 1, 1210021924, 0, 1210031035, 1, 1210031922, 0, 
1210041035, 1, 1210041920, 0, 1210051035, 1, 1210051918, 0, 1210061035, 1, 
1210061916, 0, 1210071035, 1, 1210071914, 0, 1210081035, 1, 1210081912, 0, 
1210091035, 1, 1210091911, 0, 1210101035, 1, 1210101909, 0, 1210111035, 1, 
1210111907, 0, 1210121034, 1, 1210121905, 0, 1210131034, 1, 1210131904, 0, 
1210141034, 1, 1210141902, 0, 1210151034, 1, 1210151900, 0, 1210161034, 1, 
1210161859, 0, 1210171034, 1, 1210171857, 0, 1210181034, 1, 1210181856, 0, 
1210191033, 1, 1210191854, 0, 1210201033, 1, 1210201853, 0, 1210211033, 1, 
1210211851, 0, 1210221033, 1, 1210221850, 0, 1210231033, 1, 1210231848, 0, 
1210241033, 1, 1210241847, 0, 1210251032, 1, 1210251845, 0, 1210261032, 1, 
1210261844, 0, 1210271032, 1, 1210271843, 0, 1210281032, 1, 1210281842, 0, 
1210291031, 1, 1210291840, 0, 1210301031, 1, 1210301839, 0, 1210311031, 1, 
1210311838, 0, 1211011031, 1, 1211011837, 0, 1211021030, 1, 1211021836, 0, 
1211031030, 1, 1211031835, 0, 1211041030, 1, 1211041834, 0, 1211051029, 1, 
1211051833, 0, 1211061029, 1, 1211061832, 0, 1211071028, 1, 1211071831, 0, 
1211081028, 1, 1211081830, 0, 1211091028, 1, 1211091829, 0, 1211101027, 1, 
1211101828, 0, 1211111027, 1, 1211111827, 0, 1211121026, 1, 1211121826, 0, 
1211131026, 1, 1211131826, 0, 1211141025, 1, 1211141825, 0, 1211151024, 1, 
1211151824, 0, 1211161024, 1, 1211161823, 0, 1211171023, 1, 1211171823, 0, 
1211181023, 1, 1211181822, 0, 1211191022, 1, 1211191822, 0, 1211201021, 1, 
1211201821, 0, 1211211021, 1, 1211211821, 0, 1211221020, 1, 1211221820, 0, 
1211231019, 1, 1211231820, 0, 1211241018, 1, 1211241819, 0, 1211251018, 1, 
1211251819, 0, 1211261017, 1, 1211261819, 0, 1211271016, 1, 1211271818, 0, 
1211281015, 1, 1211281818, 0, 1211291014, 1, 1211291818, 0, 1211301013, 1, 
1211301817, 0, 1212011012, 1, 1212011817, 0, 1212021011, 1, 1212021817, 0, 
1212031010, 1, 1212031817, 0, 1212041009, 1, 1212041817, 0, 1212051008, 1, 
1212051817, 0, 1212061007, 1, 1212061817, 0, 1212071006, 1, 1212071816, 0, 
1212081005, 1, 1212081816, 0, 1212091004, 1, 1212091816, 0, 1212101002, 1, 
1212101816, 0, 1212111001, 1, 1212111816, 0, 1212121000, 1, 1212121817, 0, 
1212130959, 1, 1212131817, 0, 1212140957, 1, 1212141817, 0, 1212150956, 1, 
1212151817, 0, 1212160955, 1, 1212161817, 0, 1212170953, 1, 1212171817, 0, 
1212180952, 1, 1212181817, 0, 1212190951, 1, 1212191817, 0, 1212200949, 1, 
1212201818, 0, 1212210948, 1, 1212211818, 0, 1212220946, 1, 1212221818, 0, 
1212230945, 1, 1212231818, 0, 1212240943, 1, 1212241819, 0, 1212250941, 1, 
1212251819, 0, 1212260940, 1, 1212261819, 0, 1212270938, 1, 1212271819, 0, 
1212280937, 1, 1212281820, 0, 1212290935, 1, 1212291820, 0, 1212300933, 1, 
1212301820, 0, 1212310931, 1, 1212311821, 0, 1301010930, 1, 1301011821, 0, 
1301020928, 1, 1301021821, 0, 1301030926, 1, 1301031822, 0, 1301040924, 1, 
1301041822, 0, 1301050923, 1, 1301051823, 0, 1301060921, 1, 1301061823, 0, 
1301070919, 1, 1301071823, 0, 1301080917, 1, 1301081824, 0, 1301090915, 1, 
1301091824, 0, 1301100913, 1, 1301101825, 0, 1301110911, 1, 1301111825, 0, 
1301120909, 1, 1301121825, 0, 1301130907, 1, 1301131827, 0, 1301140905, 1, 
1301141826, 0, 1301150903, 1, 1301151827, 0, 1301160901, 1, 1301161827, 0, 
1301170859, 1, 1301171828, 0, 1301180857, 1, 1301181828, 0, 1301190855, 1, 
1301191828, 0, 1301200853, 1, 1301201829, 0, 1301210851, 1, 1301211829, 0, 
1301220849, 1, 1301221830, 0, 1301230847, 1, 1301231830, 0, 1301240844, 1, 
1301241831, 0, 1301250842, 1, 1301251831, 0, 1301260840, 1, 1301261832, 0, 
1301270838, 1, 1301271832, 0, 1301280836, 1, 1301281832, 0, 1301290833, 1, 
1301291833, 0, 1301300831, 1, 1301301833, 0, 1301310829, 1, 1301311834, 0, 
1302010827, 1, 1302011834, 0, 1302020825, 1, 1302021834, 0, 1302030822, 1, 
1302031835, 0, 1302040820, 1, 1302041835, 0, 1302050818, 1, 1302051836, 0, 
1302060815, 1, 1302061836, 0, 1302070813, 1, 1302071837, 0, 1302080811, 1, 
1302081837, 0, 1302090808, 1, 1302091837, 0, 1302100806, 1, 1302101838, 0, 
1302110804, 1, 1302111838, 0, 1302120801, 1, 1302121839, 0, 1302130759, 1, 
1302131839, 0, 1302140757, 1, 1302141839, 0, 1302150754, 1, 1302151840, 0, 
1302160752, 1, 1302161840, 0, 1302170749, 1, 1302171840, 0, 1302180747, 1, 
1302181841, 0, 1302190745, 1, 1302191841, 0, 1302200742, 1, 1302201842, 0, 
1302210740, 1, 1302211842, 0, 1302220737, 1, 1302221842, 0, 1302230735, 1, 
1302231843, 0, 1302240732, 1, 1302241843, 0, 1302250730, 1, 1302251843, 0, 
1302260728, 1, 1302261844, 0, 1302270725, 1, 1302271844, 0, 1302280723, 1, 
1302281844, 0, 1303010720, 1, 1303011845, 0, 1303020718, 1, 1303021845, 0, 
1303030715, 1, 1303031845, 0, 1303040713, 1, 1303041846, 0, 1303050710, 1, 
1303051846, 0, 1303060708, 1, 1303061846, 0, 1303070705, 1, 1303071846, 0, 
1303080703, 1, 1303081847, 0, 1303090700, 1, 1303091847, 0, 1303100658, 1, 
1303101847, 0, 1303110655, 1, 1303111848, 0, 1303120653, 1, 1303121848, 0, 
1303130650, 1, 1303131848, 0, 1303140648, 1, 1303141849, 0, 1303150645, 1, 
1303151849, 0, 1303160643, 1, 1303161849, 0, 1303170640, 1, 1303171849, 0, 
1303180638, 1, 1303181850, 0, 1303190635, 1, 1303191850, 0, 1303200633, 1, 
1303201850, 0, 1303210631, 1, 1303211850, 0, 1303220628, 1, 1303221851, 0, 
1303230626, 1, 1303231851, 0, 1303240623, 1, 1303241851, 0, 1303250621, 1, 
1303251851, 0, 1303260618, 1, 1303261852, 0, 1303270616, 1, 1303271852, 0, 
1303280613, 1, 1303281852, 0, 1303290611, 1, 1303291852, 0, 1303300608, 1, 
1303301853, 0, 1303310606, 1, 1303311853, 0, 1304010603, 1, 1304011853, 0, 
1304020601, 1, 1304021853, 0, 1304030558, 1, 1304031854, 0, 1304040556, 1, 
1304041854, 0, 1304050553, 1, 1304051854, 0, 1304060551, 1, 1304061854, 0, 
1304070548, 1, 1304071854, 0, 1304080546, 1, 1304081855, 0, 1304090544, 1, 
1304091855, 0, 1304100541, 1, 1304101855, 0, 1304110539, 1, 1304111855, 0, 
1304120536, 1, 1304121855, 0, 1304130534, 1, 1304131856, 0, 1304140532, 1, 
1304141856, 0, 1304150529, 1, 1304151856, 0, 1304160527, 1, 1304161856, 0, 
1304170524, 1, 1304171856, 0, 1304180522, 1, 1304181857, 0, 1304190520, 1, 
1304191857, 0, 1304200517, 1, 1304201857, 0, 1304210515, 1, 1304211857, 0, 
1304220513, 1, 1304221857, 0, 1304230510, 1, 1304231857, 0, 1304240508, 1, 
1304241858, 0, 1304250505, 1, 1304251858, 0, 1304260503, 1, 1304261858, 0, 
1304270501, 1, 1304271858, 0, 1304280459, 1, 1304281858, 0, 1304290456, 1, 
1304291858, 0, 1304300454, 1, 1304301859, 0, 1305010452, 1, 1305011859, 0, 
1305020449, 1, 1305021859, 0, 1305030447, 1, 1305031859, 0, 1305040445, 1, 
1305041859, 0, 1305050443, 1, 1305051859, 0, 1305060441, 1, 1305061859, 0, 
1305070438, 1, 1305071859, 0, 1305080436, 1, 1305081900, 0, 1305090434, 1, 
1305091900, 0, 1305100432, 1, 1305101900, 0, 1305110430, 1, 1305111900, 0, 
1305120427, 1, 1305121900, 0, 1305130425, 1, 1305131900, 0, 1305140423, 1, 
1305141900, 0, 1305150421, 1, 1305151900, 0, 1305160419, 1, 1305161900, 0, 
1305170417, 1, 1305171900, 0, 1305180415, 1, 1305181900, 0, 1305190413, 1, 
1305191900, 0, 1305200411, 1, 1305201900, 0, 1305210409, 1, 1305211900, 0, 
1305220407, 1, 1305221900, 0, 1305230405, 1, 1305231900, 0, 1305240403, 1, 
1305241900, 0, 1305250401, 1, 1305251900, 0, 1305260359, 1, 1305261900, 0, 
1305270357, 1, 1305271900, 0, 1305280355, 1, 1305281900, 0, 1305290353, 1, 
1305291900, 0, 1305300351, 1, 1305301900, 0, 1305310349, 1, 1305311900, 0, 
1306010348, 1, 1306011900, 0, 1306020346, 1, 1306021900, 0, 1306030344, 1, 
1306031859, 0, 1306040342, 1, 1306041859, 0, 1306050340, 1, 1306051859, 0, 
1306060339, 1, 1306061859, 0, 1306070337, 1, 1306071859, 0, 1306080335, 1, 
1306081859, 0, 1306090333, 1, 1306091858, 0, 1306100332, 1, 1306101858, 0, 
1306110330, 1, 1306111858, 0, 1306120329, 1, 1306121858, 0, 1306130327, 1, 
1306131857, 0, 1306140325, 1, 1306141857, 0, 1306150324, 1, 1306151857, 0, 
1306160322, 1, 1306161856, 0, 1306170321, 1, 1306171856, 0, 1306180319, 1, 
1306181856, 0, 1306190318, 1, 1306191855, 0, 1306200316, 1, 1306201855, 0, 
1306210315, 1, 1306211854, 0, 1306220313, 1, 1306221854, 0, 1306230312, 1, 
1306231853, 0, 1306240310, 1, 1306241853, 0, 1306250309, 1, 1306251852, 0, 
1306260307, 1, 1306261852, 0, 1306270306, 1, 1306271851, 0, 1306280305, 1, 
1306281851, 0, 1306290303, 1, 1306291850, 0, 1306300302, 1, 1306301850, 0, 
1307010301, 1, 1307011849, 0, 1307020300, 1, 1307021848, 0, 1307030258, 1, 
1307031848, 0, 1307040257, 1, 1307041847, 0, 1307050256, 1, 1307051846, 0, 
1307060255, 1, 1307061845, 0, 1307070254, 1, 1307071845, 0, 1307080253, 1, 
1307081844, 0, 1307090251, 1, 1307091843, 0, 1307100250, 1, 1307101842, 0, 
1307110249, 1, 1307111841, 0, 1307120248, 1, 1307121840, 0, 1307130247, 1, 
1307131839, 0, 1307140246, 1, 1307141839, 0, 1307150245, 1, 1307151838, 0, 
1307160244, 1, 1307161837, 0, 1307170243, 1, 1307171836, 0, 1307180242, 1, 
1307181835, 0, 1307190241, 1, 1307191833, 0, 1307200240, 1, 1307201832, 0, 
1307210239, 1, 1307211831, 0, 1307220238, 1, 1307221830, 0, 1307230238, 1, 
1307231829, 0, 1307240237, 1, 1307241828, 0, 1307250236, 1, 1307251827, 0, 
1307260235, 1, 1307261825, 0, 1307270234, 1, 1307271824, 0, 1307280233, 1, 
1307281823, 0, 1307290233, 1, 1307291821, 0, 1307300232, 1, 1307301820, 0, 
1307310231, 1, 1307311819, 0, 1308010230, 1, 1308011817, 0, 1308020230, 1, 
1308021816, 0, 1308030229, 1, 1308031815, 0, 1308040228, 1, 1308041813, 0, 
1308050227, 1, 1308051812, 0, 1308060227, 1, 1308061810, 0, 1308070226, 1, 
1308071809, 0, 1308080225, 1, 1308081807, 0, 1308090225, 1, 1308091805, 0, 
1308100224, 1, 1308101804, 0, 1308110223, 1, 1308111802, 0, 1308120223, 1, 
1308121801, 0, 1308130222, 1, 1308131759, 0, 1308140221, 1, 1308141757, 0, 
1308150221, 1, 1308151756, 0, 1308160220, 1, 1308161754, 0, 1308170220, 1, 
1308171752, 0, 1308180219, 1, 1308181750, 0, 1308190218, 1, 1308191749, 0, 
1308200218, 1, 1308201747, 0, 1308210217, 1, 1308211745, 0, 1308220217, 1, 
1308221743, 0, 1308230216, 1, 1308231741, 0, 1308240216, 1, 1308241739, 0, 
1308250215, 1, 1308251737, 0, 1308260215, 1, 1308261736, 0, 1308270214, 1, 
1308271734, 0, 1308280213, 1, 1308281732, 0, 1308290213, 1, 1308291730, 0, 
1308300212, 1, 1308301728, 0, 1308310212, 1, 1308311726, 0, 1309010211, 1, 
1309011724, 0, 1309020211, 1, 1309021722, 0, 1309030210, 1, 1309031719, 0, 
1309040210, 1, 1309041717, 0, 1309050209, 1, 1309051715, 0, 1309060209, 1, 
1309061713, 0, 1309070208, 1, 1309071711, 0, 1309080208, 1, 1309081709, 0, 
1309090207, 1, 1309091707, 0, 1309100207, 1, 1309101704, 0, 1309110206, 1, 
1309111702, 0, 1309120205, 1, 1309121700, 0, 1309130205, 1, 1309131658, 0, 
1309140204, 1, 1309141656, 0, 1309150204, 1, 1309151653, 0, 1309160203, 1, 
1309161651, 0, 1309170203, 1, 1309171649, 0, 1309180202, 1, 1309181646, 0, 
1309190202, 1, 1309191644, 0, 1309200201, 1, 1309201642, 0, 1309210201, 1, 
1309211639, 0, 1309220200, 1, 1309221637, 0, 1309230200, 1, 1309231635, 0, 
1309240159, 1, 1309241632, 0, 1309250158, 1, 1309251630, 0, 1309260158, 1, 
1309261628, 0, 1309270157, 1, 1309271625, 0, 1309280157, 1, 1309281623, 0, 
1309290156, 1, 1309291620, 0, 1309300156, 1, 1309301618, 0, 1310010155, 1, 
1310011615, 0, 1310020154, 1, 1310021613, 0, 1310030154, 1, 1310031610, 0, 
1310040153, 1, 1310041608, 0, 1310050153, 1, 1310051605, 0, 1310060152, 1, 
1310061603, 0, 1310070151, 1, 1310071600, 0, 1310080151, 1, 1310081558, 0, 
1310090150, 1, 1310091555, 0, 1310100149, 1, 1310101553, 0, 1310110149, 1, 
1310111550, 0, 1310120148, 1, 1310121548, 0, 1310130147, 1, 1310131545, 0, 
1310140147, 1, 1310141542, 0, 1310150146, 1, 1310151540, 0, 1310160145, 1, 
1310161537, 0, 1310170145, 1, 1310171535, 0, 1310180144, 1, 1310181532, 0, 
1310190143, 1, 1310191529, 0, 1310200143, 1, 1310201527, 0, 1310210142, 1, 
1310211524, 0, 1310220141, 1, 1310221521, 0, 1310230141, 1, 1310231519, 0, 
1310240140, 1, 1310241516, 0, 1310250139, 1, 1310251513, 0, 1310260138, 1, 
1310261511, 0, 1310270138, 1, 1310271508, 0, 1310280137, 1, 1310281505, 0, 
1310290136, 1, 1310291503, 0, 1310300135, 1, 1310301500, 0, 1310310134, 1, 
1310311457, 0, 1311010134, 1, 1311011455, 0, 1311020133, 1, 1311021452, 0, 
1311030132, 1, 1311031449, 0, 1311040131, 1, 1311041446, 0, 1311050130, 1, 
1311051444, 0, 1311060130, 1, 1311061441, 0, 1311070129, 1, 1311071438, 0, 
1311080128, 1, 1311081435, 0, 1311090127, 1, 1311091433, 0, 1311100126, 1, 
1311101430, 0, 1311110125, 1, 1311111427, 0, 1311120124, 1, 1311121424, 0, 
1311130123, 1, 1311131422, 0, 1311140122, 1, 1311141419, 0, 1311150122, 1, 
1311151416, 0, 1311160121, 1, 1311161413, 0, 1311170120, 1, 1311171410, 0, 
1311180119, 1, 1311181408, 0, 1311190118, 1, 1311191405, 0, 1311200117, 1, 
1311201402, 0, 1311210116, 1, 1311211359, 0, 1311220115, 1, 1311221356, 0, 
1311230114, 1, 1311231353, 0, 1311240113, 1, 1311241351, 0, 1311250112, 1, 
1311251348, 0, 1311260111, 1, 1311261345, 0, 1311270110, 1, 1311271342, 0, 
1311280109, 1, 1311281339, 0, 1311290108, 1, 1311291336, 0, 1311300107, 1, 
1311301334, 0, 1312010105, 1, 1312011331, 0, 1312020104, 1, 1312021328, 0, 
1312030103, 1, 1312031325, 0, 1312040102, 1, 1312041322, 0, 1312050101, 1, 
1312051319, 0, 1312060100, 1, 1312061316, 0, 1312070059, 1, 1312071313, 0, 
1312080057, 1, 1312081311, 0, 1312090056, 1, 1312091308, 0, 1312100055, 1, 
1312101305, 0, 1312110054, 1, 1312111302, 0, 1312120053, 1, 1312121259, 0, 
1312130051, 1, 1312131256, 0, 1312140050, 1, 1312141253, 0, 1312150049, 1, 
1312151250, 0, 1312160048, 1, 1312161247, 0, 1312170046, 1, 1312171244, 0, 
1312180045, 1, 1312181241, 0, 1312190044, 1, 1312191238, 0, 1312200042, 1, 
1312201235, 0, 1312210041, 1, 1312211233, 0, 1312220039, 1, 1312221230, 0, 
1312230038, 1, 1312231227, 0, 1312240037, 1, 1312241224, 0, 1312250035, 1, 
1312251221, 0, 1312260034, 1, 1312261218, 0, 1312270032, 1, 1312271215, 0, 
1312280031, 1, 1312281212, 0, 1312290029, 1, 1312291209, 0, 1312300028, 1, 
1312301206, 0, 1312310026, 1, 1312311203, 0
};


int checker(long DATE) {
    int currentValue = 0;
    for (int i=0; i < marsLength; i = i + 2) {
        horizonTimeLong = pgm_read_dword_near(mars + i);
        if (horizonTimeLong <= DATE) {
            lastEvent = horizonTimeLong;
            currentValue = pgm_read_dword_near(mars + i + 1);
        }
    }
    Serial.print("Last event was  ");
    Serial.println(lastEvent);
    return currentValue;

}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
    return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
    return ( (val/16*10) + (val%16) );
}

// Convert binary coded decimal to normal decimal numbers
long bcdToLong(byte val)
{
    return ( (val/16*10) + (val%16) );
}



// Gets the date and time from the ds1307
void getDateDs1307(long *second,
long *minute,
long *hour,
long *dayOfWeek,
long *dayOfMonth,
long *month,
long *year)
{

    // Reset the register pointer
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(0);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

    // A few of these need masks because certain bits are control bits
    *second     = bcdToLong(Wire.read() & 0x7f);
    *minute     = bcdToLong(Wire.read());
    *hour       = bcdToLong(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
    *dayOfWeek  = bcdToLong(Wire.read());
    *dayOfMonth = bcdToLong(Wire.read());
    *month      = bcdToLong(Wire.read());
    *year       = bcdToLong(Wire.read());
}




void setup() {


    myservo.attach(9);

    delay(5000);

    pinMode(ledPin, OUTPUT);
    Serial.begin(9600); 
    Wire.begin();
    Serial.println("Starting...");

    myservo.write(DOWN);
    Serial.println("Servo should be DOWN");
    delay(2000);
    myservo.write(UP);
    Serial.println("Servo should be at UP");
    delay(2000);
    myservo.write(DOWN);
    Serial.println("Servo should be at DOWN");
    delay(2000);
    myservo.write(UP);
    Serial.println("Servo should be at UP");
    delay(2000);
    myservo.write(DOWN);
    Serial.println("Servo should be at DOWN");
    delay(2000);



}

void loop() {

    long second, minute, hour, dayOfWeek, dayOfMonth, month, year;

    getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

    Serial.print(hour, DEC);
    Serial.print(":");
    if (minute < 10) {
        Serial.print("0");
    }
    Serial.print(minute, DEC);
    Serial.print(":");
    if (second < 10) {
        Serial.print("0");
    }
    Serial.print(second, DEC);
    Serial.print("  ");
    Serial.print(month, DEC);
    Serial.print("/");
    if (dayOfMonth < 10) {
        Serial.print("0");
    }
    Serial.print(dayOfMonth, DEC);
    Serial.print("/");
    Serial.print(year + 2000, DEC);
    Serial.print("  Day_of_week:");
    Serial.println(dayOfWeek, DEC);

    //7 12 16 33

    convertedDate = year * 100000000;
    convertedDate = convertedDate + (month * 1000000);
    convertedDate = convertedDate + (dayOfMonth * 10000);
    convertedDate = convertedDate + (hour * 100);
    convertedDate = convertedDate + (minute);
    Serial.print("Current time is ");
    Serial.println(convertedDate);


    MARS = checker(convertedDate);
    
    Serial.print("I think Mars should be ");
    Serial.println(MARS);
    Serial.println();
    
    if (MARS == 0) {
        myservo.write(DOWN);
    } 
    else {
        myservo.write(UP);
    }

    delay(5000);

}


