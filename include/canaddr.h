#ifndef CANADDR_H_
#define CANADDR_H_

#define CA_MAIN 0x0C /* MAIN controller                              */
#define CA_TLS0 0x16 /* TiLt Sensor for levelling                    */
#define CA_TLS1 0x19 /* TiLt Sensor for levelling as backup          */
#define CA_VSL0 0x13 /* ViSual Locator on the front                  */
#define CA_VSL1 0x15 /* ViSual Locator on the back                   */
#define CA_PSU  0x73 /* Power Supply Unit                            */
#define CA_SWH0 0x49 /* SWing arm of Horizontal on the front left    */
#define CA_SWH1 0x4A /* SWing arm of Horizontal on the back left     */
#define CA_SWH2 0x4F /* SWing arm of Horizontal on the back right    */
#define CA_SWH3 0x4C /* SWing arm of Horizontal on the front right   */
#define CA_RSE0 0x39 /* RaiSE arm on the front left                  */
#define CA_RSE1 0x3A /* RaiSE arm on the back left                   */
#define CA_RSE2 0x3F /* RaiSE arm on the back right                  */
#define CA_RSE3 0x3C /* RaiSE arm on the front right                 */
#define CA_SWV0 0x40 /* SWing leg of Vertical on the front left      */
#define CA_SWV1 0x43 /* SWing leg of Vertical on the back left       */
#define CA_SWV2 0x46 /* SWing leg of Vertical on the back right      */
#define CA_SWV3 0x45 /* SWing leg of Vertical on the front right     */
#define CA_PRP0 0x30 /* PRoP on the front left                       */
#define CA_PRP1 0x33 /* PRoP on the back left                        */
#define CA_PRP2 0x36 /* PRoP on the back right                       */
#define CA_PRP3 0x35 /* PRoP on the front right                      */
#define CA_X0   0x2F /* crane on the front for X-axis                */
#define CA_X1   0x2C /* crane on the back for X-axis                 */
#define CA_Y0   0x2A /* crane on the front for Y-axis outside        */
#define CA_Y1   0x29 /* crane on the front for Y-axis inside         */
#define CA_Y2   0x25 /* crane on the back for Y-axis inside          */
#define CA_Y3   0x26 /* crane on the back for Y-axis outside         */
#define CA_Z0   0x23 /* crane on the front for Z-axis                */
#define CA_Z1   0x20 /* crane on the back for Z-axis                 */
#define CA_SDT  0x93 /* ShielD of Top                                */
#define CA_SDS0 0xA3 /* ShielD of Side outside on the left           */
#define CA_SDS1 0xA5 /* ShielD of Side inside on the left            */
#define CA_SDS2 0xA9 /* ShielD of Side inside on the right           */
#define CA_SDS3 0xA6 /* ShielD of Side outside on the right          */
#define CA_SDF0 0xB0 /* ShielD of Front on the far left              */
#define CA_SDF1 0xB3 /* ShielD of Front on the left                  */
#define CA_SDF2 0xB5 /* ShielD of Front on the right                 */
#define CA_SDF3 0xB6 /* ShielD of Front on the far right             */
#define CA_SDB0 0xB9 /* ShielD of Back on the far left               */
#define CA_SDB1 0xBA /* ShielD of Back on the left                   */
#define CA_SDB2 0xBC /* ShielD of Back on the right                  */
#define CA_SDB3 0xBF /* ShielD of Back on the far right              */
#define CA_MOM0 0x83 /* MOMent electric machinery on the front left  */
#define CA_MOM1 0x85 /* MOMent electric machinery on the back left   */
#define CA_MOM2 0x89 /* MOMent electric machinery on the back right  */
#define CA_MOM3 0x86 /* MOMent electric machinery on the front right */
#define CA_GEN0 0x52 /* GENerator of Diesel                          */
#define CA_GEN1 0x66 /* GENerator of Shaft                           */
#define CA_GW   0xFE /* network GateWay                              */

#endif /* CANADDR_H_ */
