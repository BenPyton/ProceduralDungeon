"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[40781],{15993:(e,r,n)=>{n.r(r),n.d(r,{assets:()=>l,contentTitle:()=>i,default:()=>a,frontMatter:()=>t,metadata:()=>s,toc:()=>c});const s=JSON.parse('{"id":"api/Classes/Door/Door","title":"Door","description":"Procedural Dungeon \u23f5","source":"@site/docs/api/Classes/Door/Door.md","sourceDirName":"api/Classes/Door","slug":"/api/Classes/Door/","permalink":"/ProceduralDungeon/api/Classes/Door/","draft":false,"unlisted":false,"tags":[],"version":"current","frontMatter":{"pagination_prev":null,"pagination_next":null},"sidebar":"homeSidebar"}');var o=n(74848),d=n(28453);const t={pagination_prev:null,pagination_next:null},i="Door",l={},c=[{value:"Class Details",id:"class-details",level:2},{value:"Description",id:"description",level:2},{value:"Events",id:"events",level:2},{value:"Properties",id:"properties",level:2},{value:"Nodes",id:"nodes",level:2}];function h(e){const r={a:"a",br:"br",code:"code",em:"em",h1:"h1",h2:"h2",header:"header",p:"p",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,d.R)(),...e.components};return(0,o.jsxs)(o.Fragment,{children:[(0,o.jsxs)(r.p,{children:[(0,o.jsx)(r.a,{href:"/ProceduralDungeon/api/",children:"Procedural Dungeon"})," \u23f5\nDoor"]}),"\n",(0,o.jsx)(r.header,{children:(0,o.jsx)(r.h1,{id:"door",children:"Door"})}),"\n",(0,o.jsx)(r.h2,{id:"class-details",children:"Class Details"}),"\n",(0,o.jsxs)(r.p,{children:[(0,o.jsx)(r.strong,{children:"Defined in:"})," ",(0,o.jsx)(r.code,{children:"Public/Door.h"}),(0,o.jsx)(r.br,{}),"\n",(0,o.jsx)(r.strong,{children:"Hierarchy:"})," ",(0,o.jsx)(r.em,{children:"Object \u2192 Actor \u2192 Door"}),(0,o.jsx)(r.br,{}),"\n",(0,o.jsx)(r.strong,{children:"Exposed in blueprint as:"})," Blueprint\xa0Base\xa0Class | Variable\xa0Type"]}),"\n",(0,o.jsx)(r.h2,{id:"description",children:"Description"}),"\n",(0,o.jsxs)(r.p,{children:["Base class for all door actors in the dungeon.",(0,o.jsx)("br",{}),"Use this class even if you want to create a wall to place instead of a door (when the door is not connected to another room for example)."]}),"\n",(0,o.jsx)(r.h2,{id:"events",children:"Events"}),"\n",(0,o.jsx)(r.table,{children:(0,o.jsx)(r.thead,{children:(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.th,{children:"Name"}),(0,o.jsx)(r.th,{children:"Category"}),(0,o.jsx)(r.th,{children:"Description"})]})})}),"\n",(0,o.jsx)(r.h2,{id:"properties",children:"Properties"}),"\n",(0,o.jsxs)(r.table,{children:[(0,o.jsx)(r.thead,{children:(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.th,{children:"Name"}),(0,o.jsx)(r.th,{children:"Type"}),(0,o.jsx)(r.th,{children:"Category"}),(0,o.jsx)(r.th,{children:"Accessors"}),(0,o.jsx)(r.th,{children:"Description"})]})}),(0,o.jsxs)(r.tbody,{children:[(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.td,{children:(0,o.jsx)(r.a,{href:"/ProceduralDungeon/api/Door/bAlwaysUnlocked",children:"Always Unlocked"})}),(0,o.jsx)(r.td,{children:"bool"}),(0,o.jsx)(r.td,{children:"Door"}),(0,o.jsxs)(r.td,{children:["Blueprint\xa0Read/Write",(0,o.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,o.jsx)(r.td,{})]}),(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.td,{children:(0,o.jsx)(r.a,{href:"/ProceduralDungeon/api/Door/bAlwaysVisible",children:"Always Visible"})}),(0,o.jsx)(r.td,{children:"bool"}),(0,o.jsx)(r.td,{children:"Door"}),(0,o.jsxs)(r.td,{children:["Blueprint\xa0Read/Write",(0,o.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,o.jsx)(r.td,{})]}),(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.td,{children:"Default\xa0Scene\xa0Component"}),(0,o.jsx)(r.td,{children:"USceneComponent*"}),(0,o.jsx)(r.td,{children:"Door"}),(0,o.jsx)(r.td,{children:"Edit\xa0Anywhere"}),(0,o.jsx)(r.td,{})]}),(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.td,{children:(0,o.jsx)(r.a,{href:"/ProceduralDungeon/api/Door/Type",children:"Door Type"})}),(0,o.jsx)(r.td,{children:"UDoorType*"}),(0,o.jsx)(r.td,{children:"Door"}),(0,o.jsxs)(r.td,{children:["Blueprint\xa0Read\xa0Only",(0,o.jsx)("br",{}),"Edit\xa0Defaults\xa0Only"]}),(0,o.jsx)(r.td,{})]}),(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.td,{children:(0,o.jsx)(r.a,{href:"/ProceduralDungeon/api/Door/RoomA",children:"Room A"})}),(0,o.jsx)(r.td,{children:"URoom*"}),(0,o.jsx)(r.td,{children:"Door"}),(0,o.jsx)(r.td,{children:"Blueprint\xa0Read\xa0Only"}),(0,o.jsx)(r.td,{children:"The two connected rooms to this door"})]}),(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.td,{children:(0,o.jsx)(r.a,{href:"/ProceduralDungeon/api/Door/RoomB",children:"Room B"})}),(0,o.jsx)(r.td,{children:"URoom*"}),(0,o.jsx)(r.td,{children:"Door"}),(0,o.jsx)(r.td,{children:"Blueprint\xa0Read\xa0Only"}),(0,o.jsx)(r.td,{})]})]})]}),"\n",(0,o.jsx)(r.h2,{id:"nodes",children:"Nodes"}),"\n",(0,o.jsxs)(r.table,{children:[(0,o.jsx)(r.thead,{children:(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.th,{children:"Name"}),(0,o.jsx)(r.th,{children:"Category"}),(0,o.jsx)(r.th,{children:"Description"})]})}),(0,o.jsxs)(r.tbody,{children:[(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.td,{children:(0,o.jsx)(r.a,{href:"/ProceduralDungeon/api/Door/IsLocked",children:"Is Locked"})}),(0,o.jsx)(r.td,{children:"Door"}),(0,o.jsx)(r.td,{})]}),(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.td,{children:(0,o.jsx)(r.a,{href:"/ProceduralDungeon/api/Door/IsOpen",children:"Is Open"})}),(0,o.jsx)(r.td,{children:"Door"}),(0,o.jsx)(r.td,{})]}),(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.td,{children:(0,o.jsx)(r.a,{href:"/ProceduralDungeon/api/Door/Lock",children:"Lock"})}),(0,o.jsx)(r.td,{children:"Door"}),(0,o.jsx)(r.td,{})]}),(0,o.jsxs)(r.tr,{children:[(0,o.jsx)(r.td,{children:(0,o.jsx)(r.a,{href:"/ProceduralDungeon/api/Door/Open",children:"Open"})}),(0,o.jsx)(r.td,{children:"Door"}),(0,o.jsx)(r.td,{})]})]})]})]})}function a(e={}){const{wrapper:r}={...(0,d.R)(),...e.components};return r?(0,o.jsx)(r,{...e,children:(0,o.jsx)(h,{...e})}):h(e)}},28453:(e,r,n)=>{n.d(r,{R:()=>t,x:()=>i});var s=n(96540);const o={},d=s.createContext(o);function t(e){const r=s.useContext(d);return s.useMemo((function(){return"function"==typeof e?e(r):{...r,...e}}),[r,e])}function i(e){let r;return r=e.disableParentContext?"function"==typeof e.components?e.components(o):e.components||o:t(e.components),s.createElement(d.Provider,{value:r},e.children)}}}]);