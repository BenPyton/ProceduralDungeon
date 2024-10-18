"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[441],{44398:(e,n,t)=>{t.r(n),t.d(n,{assets:()=>l,contentTitle:()=>o,default:()=>a,frontMatter:()=>r,metadata:()=>d,toc:()=>c});var s=t(74848),i=t(28453);const r={pagination_prev:"guides/Installation",pagination_next:"guides/Creating-Rooms"},o="Plugin Settings",d={id:"guides/Plugin-Settings",title:"Plugin Settings",description:"The plugin adds several project-wide settings to allow you to customize it to your needs.",source:"@site/versioned_docs/version-3.3/guides/Plugin-Settings.md",sourceDirName:"guides",slug:"/guides/Plugin-Settings",permalink:"/ProceduralDungeon/3.3/guides/Plugin-Settings",draft:!1,unlisted:!1,tags:[],version:"3.3",frontMatter:{pagination_prev:"guides/Installation",pagination_next:"guides/Creating-Rooms"},sidebar:"homeSidebar",previous:{title:"Installation",permalink:"/ProceduralDungeon/3.3/guides/Installation"},next:{title:"Creating Rooms",permalink:"/ProceduralDungeon/3.3/guides/Creating-Rooms"}},l={},c=[{value:"Runtime Settings",id:"runtime-settings",level:2},{value:"General",id:"general",level:3},{value:"Occlusion Culling",id:"occlusion-culling",level:3},{value:"Debug",id:"debug",level:3},{value:"Editor Settings",id:"editor-settings",level:2},{value:"General",id:"general-1",level:3},{value:"Room Editor Mode",id:"room-editor-mode",level:3}];function h(e){const n={a:"a",code:"code",em:"em",h1:"h1",h2:"h2",h3:"h3",header:"header",img:"img",p:"p",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,i.R)(),...e.components};return(0,s.jsxs)(s.Fragment,{children:[(0,s.jsx)(n.header,{children:(0,s.jsx)(n.h1,{id:"plugin-settings",children:"Plugin Settings"})}),"\n",(0,s.jsx)(n.p,{children:"The plugin adds several project-wide settings to allow you to customize it to your needs."}),"\n",(0,s.jsx)(n.p,{children:"The settings are split in 2 places, one for the runtime module and one for the editor module."}),"\n",(0,s.jsx)(n.h2,{id:"runtime-settings",children:"Runtime Settings"}),"\n",(0,s.jsxs)(n.p,{children:["You can find the runtime settings in the ",(0,s.jsx)(n.code,{children:"Project Settings"})," -> ",(0,s.jsx)(n.code,{children:"Plugins"})," -> ",(0,s.jsx)(n.code,{children:"Procedural Dungeon"})," (see screenshot below)."]}),"\n",(0,s.jsx)(n.p,{children:(0,s.jsx)(n.img,{src:t(72757).A+"",width:"1185",height:"705"})}),"\n",(0,s.jsx)(n.h3,{id:"general",children:"General"}),"\n",(0,s.jsxs)(n.table,{children:[(0,s.jsx)(n.thead,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.th,{children:"Setting"}),(0,s.jsx)(n.th,{children:"Description"})]})}),(0,s.jsxs)(n.tbody,{children:[(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Room\xa0Unit"})}),(0,s.jsx)(n.td,{children:"This is the size of a 1 unit room. Each room have a size made of a multiple of this unit. For instance, if you have a room unit of (1000, 1000, 400) and a room of size (3, 1, 2), this room will have in world unit a size of (3000, 1000, 800)."})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Default\xa0Door\xa0Size"})}),(0,s.jsx)(n.td,{children:"This is the size used by the debug draw to draw the doors. This is just a visual indication for you, it is not used during the generation process."})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Door\xa0Offset"})}),(0,s.jsx)(n.td,{children:"You can add an offset on the Z-axis for the doors (e.g. if you want to place a ground with a non-zero depth). This offset is a percentage between 0 and 1 of the room unit height."})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Can\xa0Loop"})}),(0,s.jsxs)(n.td,{children:["Enable possibility to link doors when they can.",(0,s.jsx)("br",{}),"[",(0,s.jsx)(n.em,{children:"DEPRECATED"}),"] This setting is deprecated and will be removed in a future version.",(0,s.jsx)("br",{}),"Use the ",(0,s.jsx)(n.code,{children:"Can Loop"})," variable in the ",(0,s.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Dungeon-Generator",children:"Dungeon Generator"})," instead.",(0,s.jsx)("br",{}),"As long as this setting remains, it will act as a global toggle for the feature.",(0,s.jsx)("br",{}),"You should keep it ticked if you want to get the same behavior as when it will be removed."]})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Room\xa0Object\xa0Type"})}),(0,s.jsxs)(n.td,{children:["The Object Type used by the room trigger box to test actor overlaps.",(0,s.jsx)("br",{}),"By default it is set to ",(0,s.jsx)(n.code,{children:"ECC_EngineTraceChannel6"}),", you cannot set that channel from the editor but you can from the ",(0,s.jsx)(n.code,{children:"DefaultGame.ini"})," file if you want to revert back to the default value.",(0,s.jsx)("br",{}),"You can create new ",(0,s.jsx)(n.a,{href:"https://docs.unrealengine.com/4.27/en-US/InteractiveExperiences/Physics/Collision/HowTo/AddCustomCollisionType/",children:"Object Type Channels"})," from your project settings if you want a special channel for the dungeon rooms, and have controls on which actors can interact with them."]})]})]})]}),"\n",(0,s.jsx)(n.p,{children:(0,s.jsx)(n.img,{src:t(5408).A+"",width:"817",height:"662"})}),"\n",(0,s.jsx)(n.h3,{id:"occlusion-culling",children:"Occlusion Culling"}),"\n",(0,s.jsx)(n.p,{children:(0,s.jsxs)(n.em,{children:["Note: These settings are badly named and should be referred as ",(0,s.jsx)(n.code,{children:"Room Culling"})," instead. It will be changed in a future version."]})}),"\n",(0,s.jsxs)(n.table,{children:[(0,s.jsx)(n.thead,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.th,{children:"Setting"}),(0,s.jsx)(n.th,{children:"Description"})]})}),(0,s.jsxs)(n.tbody,{children:[(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Enable\xa0Occlusion\xa0Culling"})}),(0,s.jsxs)(n.td,{children:["This enables the occlusion culling system (see ",(0,s.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Occlusion-Culling",children:"Occlusion Culling"})," for more details)."]})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Occlusion\xa0Distance"})}),(0,s.jsx)(n.td,{children:"Define the distance (in number of connected rooms) of visible rooms from the player's room."})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Occlude\xa0Dynamic\xa0Actors"})}),(0,s.jsxs)(n.td,{children:["Set it if you want to have dynamic actors with a ",(0,s.jsx)(n.code,{children:"RoomVisibility"})," component tracked in the rooms to show/hide them with the room occlusions (see ",(0,s.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Occlusion-Culling",children:"Occlusion Culling"})," for more details)."]})]})]})]}),"\n",(0,s.jsx)(n.h3,{id:"debug",children:"Debug"}),"\n",(0,s.jsxs)(n.table,{children:[(0,s.jsx)(n.thead,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.th,{children:"Setting"}),(0,s.jsx)(n.th,{children:"Description"})]})}),(0,s.jsxs)(n.tbody,{children:[(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Draw\xa0Debug"})}),(0,s.jsx)(n.td,{children:"This will display the debug draw of room bounding volume and doors volume in editor and development builds."})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Show\xa0Room\xa0Origin"})}),(0,s.jsxs)(n.td,{children:["If ticked, shows the room's local origin (0,0,0) with a magenta octahedron.",(0,s.jsx)("br",{}),"[",(0,s.jsx)(n.em,{children:"DEPRECATED"}),"] This setting is deprecated and will be removed in a future version (unless someone considers that it's really useful)."]})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Flip\xa0Door\xa0Arrow\xa0Side"})}),(0,s.jsx)(n.td,{children:"If ticked, the door arrows will point to the inside of the room instead of the outside."})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Door\xa0Arrow\xa0Length"})}),(0,s.jsx)(n.td,{children:"The length of the door arrow."})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Door\xa0Arrow\xa0Head\xa0Size"})}),(0,s.jsx)(n.td,{children:"The size of the arrow head."})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"On\xa0Screen\xa0Print\xa0Debug"})}),(0,s.jsx)(n.td,{children:"This will prints some debug messages on the player screen in addition to the logs (useful to compare seeds when testing multiplayer)."})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Print\xa0Debug\xa0Duration"})}),(0,s.jsx)(n.td,{children:"The duration in seconds of the on screen debug messages."})]})]})]}),"\n",(0,s.jsx)(n.h2,{id:"editor-settings",children:"Editor Settings"}),"\n",(0,s.jsxs)(n.p,{children:["You can find the editor related settings in the ",(0,s.jsx)(n.code,{children:"Project Settings"})," -> ",(0,s.jsx)(n.code,{children:"Editor"})," -> ",(0,s.jsx)(n.code,{children:"Procedural Dungeon"})," (see screenshot below)."]}),"\n",(0,s.jsx)(n.p,{children:(0,s.jsx)(n.img,{src:t(30290).A+"",width:"1183",height:"629"})}),"\n",(0,s.jsx)(n.h3,{id:"general-1",children:"General"}),"\n",(0,s.jsxs)(n.table,{children:[(0,s.jsx)(n.thead,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.th,{children:"Setting"}),(0,s.jsx)(n.th,{children:"Description"})]})}),(0,s.jsxs)(n.tbody,{children:[(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Default\xa0Room\xa0Data"})}),(0,s.jsxs)(n.td,{children:["The default selected ",(0,s.jsx)(n.code,{children:"Room Data"})," class in the class picker when creating a new asset from content browser."]})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Use\xa0Automatically\xa0Default\xa0Class If\xa0No\xa0Child"})}),(0,s.jsxs)(n.td,{children:["If the ",(0,s.jsx)(n.code,{children:"Room Data"})," class set in the ",(0,s.jsx)(n.code,{children:"Default Room Data"})," setting above has no child classes, then the class picker will not be displayed and an asset is created using the ",(0,s.jsx)(n.code,{children:"Default Room Data"})," class directly."]})]})]})]}),"\n",(0,s.jsx)(n.h3,{id:"room-editor-mode",children:"Room Editor Mode"}),"\n",(0,s.jsxs)(n.table,{children:[(0,s.jsx)(n.thead,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.th,{children:"Setting"}),(0,s.jsx)(n.th,{children:"Description"})]})}),(0,s.jsxs)(n.tbody,{children:[(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Default Margins"})}),(0,s.jsx)(n.td,{children:"This is the default margins values for the volume utility function when activating the Room Editor mode."})]}),(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:(0,s.jsx)(n.strong,{children:"Use\xa0the\xa0room\xa0bounds\xa0center as\xa0pivot\xa0for\xa0camera\xa0orbit"})}),(0,s.jsxs)(n.td,{children:["When ",(0,s.jsx)(n.code,{children:"Orbit Camera Around Selection"})," is ticked in your Editor Preferences and this setting is also ticked, then the pivot for orbit rotation of the camera in the Room Editor mode will use the room's bound center instead of the current selection."]})]})]})]})]})}function a(e={}){const{wrapper:n}={...(0,i.R)(),...e.components};return n?(0,s.jsx)(n,{...e,children:(0,s.jsx)(h,{...e})}):h(e)}},30290:(e,n,t)=>{t.d(n,{A:()=>s});const s=t.p+"assets/images/PluginEditorSettings-aa91d5293ae07824ff2c19bad60692b5.jpg"},5408:(e,n,t)=>{t.d(n,{A:()=>s});const s=t.p+"assets/images/PluginRoomSettings-ac5fdc46d1c8e70d873e43e96a6a121b.jpg"},72757:(e,n,t)=>{t.d(n,{A:()=>s});const s=t.p+"assets/images/PluginSettings-dbdc25b2dec38bc7c57975be6e99b57b.jpg"},28453:(e,n,t)=>{t.d(n,{R:()=>o,x:()=>d});var s=t(96540);const i={},r=s.createContext(i);function o(e){const n=s.useContext(r);return s.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function d(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(i):e.components||i:o(e.components),s.createElement(r.Provider,{value:n},e.children)}}}]);