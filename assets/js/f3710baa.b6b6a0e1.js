"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[1658],{2078:(e,n,s)=>{s.r(n),s.d(n,{assets:()=>r,contentTitle:()=>i,default:()=>h,frontMatter:()=>d,metadata:()=>c,toc:()=>a});var t=s(4848),o=s(8453);const d={slug:"/api/Room/IsLocked",pagination_prev:null,pagination_next:null},i="Is Locked",c={id:"api/Classes/Room/Nodes/IsLocked/IsLocked",title:"IsLocked",description:"Procedural Dungeon \u23f5",source:"@site/docs/api/Classes/Room/Nodes/IsLocked/IsLocked.md",sourceDirName:"api/Classes/Room/Nodes/IsLocked",slug:"/api/Room/IsLocked",permalink:"/ProceduralDungeon/api/Room/IsLocked",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{slug:"/api/Room/IsLocked",pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},r={},a=[{value:"Description",id:"description",level:2},{value:"Inputs",id:"inputs",level:2},{value:"Outputs",id:"outputs",level:2}];function l(e){const n={a:"a",code:"code",h1:"h1",h2:"h2",img:"img",p:"p",pre:"pre",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,o.R)(),...e.components};return(0,t.jsxs)(t.Fragment,{children:[(0,t.jsxs)(n.p,{children:[(0,t.jsx)(n.a,{href:"/ProceduralDungeon/api/",children:"Procedural Dungeon"})," \u23f5\n",(0,t.jsx)(n.a,{href:"/ProceduralDungeon/api/Classes/Room/",children:"Room"})," \u23f5\nIs Locked"]}),"\n",(0,t.jsx)(n.h1,{id:"is-locked",children:"Is Locked"}),"\n",(0,t.jsxs)(n.p,{children:[(0,t.jsx)(n.strong,{children:"Category:"})," Room"]}),"\n",(0,t.jsx)(n.h2,{id:"description",children:"Description"}),"\n",(0,t.jsxs)(n.p,{children:["Is the room locked?",(0,t.jsx)("br",{}),"If it is, the doors will be locked (except if they have ",(0,t.jsx)(n.code,{children:"Alway Unlocked"}),")."]}),"\n",(0,t.jsx)(n.p,{children:"Node"}),"\n",(0,t.jsx)(n.p,{children:(0,t.jsx)(n.img,{src:s(2410).A+"",width:"233",height:"38"})}),"\n",(0,t.jsx)(n.p,{children:"C++"}),"\n",(0,t.jsx)(n.pre,{children:(0,t.jsx)(n.code,{className:"language-cpp",children:"bool IsLocked() const\n"})}),"\n",(0,t.jsx)(n.h2,{id:"inputs",children:"Inputs"}),"\n",(0,t.jsxs)(n.table,{children:[(0,t.jsx)(n.thead,{children:(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.th,{children:"Name"}),(0,t.jsx)(n.th,{children:"Type"}),(0,t.jsx)(n.th,{children:"Description"})]})}),(0,t.jsx)(n.tbody,{children:(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.td,{children:"Target"}),(0,t.jsx)(n.td,{children:"Room Object Reference"}),(0,t.jsx)(n.td,{})]})})]}),"\n",(0,t.jsx)(n.h2,{id:"outputs",children:"Outputs"}),"\n",(0,t.jsxs)(n.table,{children:[(0,t.jsx)(n.thead,{children:(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.th,{children:"Name"}),(0,t.jsx)(n.th,{children:"Type"}),(0,t.jsx)(n.th,{children:"Description"})]})}),(0,t.jsx)(n.tbody,{children:(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.td,{children:"Return Value"}),(0,t.jsx)(n.td,{children:"Boolean"}),(0,t.jsxs)(n.td,{children:["Is the room locked?",(0,t.jsx)("br",{}),"If it is, the doors will be locked (except if they have ",(0,t.jsx)(n.code,{children:"Alway Unlocked"}),")."]})]})})]})]})}function h(e={}){const{wrapper:n}={...(0,o.R)(),...e.components};return n?(0,t.jsx)(n,{...e,children:(0,t.jsx)(l,{...e})}):l(e)}},2410:(e,n,s)=>{s.d(n,{A:()=>t});const t="data:image/png;base64,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"},8453:(e,n,s)=>{s.d(n,{R:()=>i,x:()=>c});var t=s(6540);const o={},d=t.createContext(o);function i(e){const n=t.useContext(d);return t.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function c(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(o):e.components||o:i(e.components),t.createElement(d.Provider,{value:n},e.children)}}}]);