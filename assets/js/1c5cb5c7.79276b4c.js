"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[6706],{6837:(e,n,r)=>{r.r(n),r.d(n,{assets:()=>i,contentTitle:()=>d,default:()=>h,frontMatter:()=>t,metadata:()=>c,toc:()=>l});var s=r(4848),o=r(8453);const t={slug:"/api/Door/IsOpen"},d="Is Open",c={id:"api/Classes/Door/Nodes/IsOpen/IsOpen",title:"IsOpen",description:"Procedural Dungeon \u23f5",source:"@site/docs/api/Classes/Door/Nodes/IsOpen/IsOpen.md",sourceDirName:"api/Classes/Door/Nodes/IsOpen",slug:"/api/Door/IsOpen",permalink:"/ProceduralDungeon/api/Door/IsOpen",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{slug:"/api/Door/IsOpen"},sidebar:"homeSidebar",previous:{title:"IsLocked",permalink:"/ProceduralDungeon/api/Door/IsLocked"},next:{title:"Lock",permalink:"/ProceduralDungeon/api/Door/Lock"}},i={},l=[{value:"Inputs",id:"inputs",level:2},{value:"Outputs",id:"outputs",level:2}];function a(e){const n={a:"a",code:"code",h1:"h1",h2:"h2",img:"img",p:"p",pre:"pre",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,o.R)(),...e.components};return(0,s.jsxs)(s.Fragment,{children:[(0,s.jsxs)(n.p,{children:[(0,s.jsx)(n.a,{href:"/ProceduralDungeon/api/",children:"Procedural Dungeon"})," \u23f5\n",(0,s.jsx)(n.a,{href:"/ProceduralDungeon/api/Classes/Door/",children:"Door"})," \u23f5\nIs Open"]}),"\n",(0,s.jsx)(n.h1,{id:"is-open",children:"Is Open"}),"\n",(0,s.jsxs)(n.p,{children:[(0,s.jsx)(n.strong,{children:"Category:"})," Door"]}),"\n",(0,s.jsx)(n.p,{children:"Node"}),"\n",(0,s.jsx)(n.p,{children:(0,s.jsx)(n.img,{src:r(2409).A+"",width:"233",height:"38"})}),"\n",(0,s.jsx)(n.p,{children:"C++"}),"\n",(0,s.jsx)(n.pre,{children:(0,s.jsx)(n.code,{className:"language-cpp",children:"bool IsOpen()\n"})}),"\n",(0,s.jsx)(n.h2,{id:"inputs",children:"Inputs"}),"\n",(0,s.jsxs)(n.table,{children:[(0,s.jsx)(n.thead,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.th,{children:"Name"}),(0,s.jsx)(n.th,{children:"Type"}),(0,s.jsx)(n.th,{children:"Description"})]})}),(0,s.jsx)(n.tbody,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:"Target"}),(0,s.jsx)(n.td,{children:"Door Object Reference"}),(0,s.jsx)(n.td,{})]})})]}),"\n",(0,s.jsx)(n.h2,{id:"outputs",children:"Outputs"}),"\n",(0,s.jsxs)(n.table,{children:[(0,s.jsx)(n.thead,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.th,{children:"Name"}),(0,s.jsx)(n.th,{children:"Type"}),(0,s.jsx)(n.th,{children:"Description"})]})}),(0,s.jsx)(n.tbody,{children:(0,s.jsxs)(n.tr,{children:[(0,s.jsx)(n.td,{children:"Return Value"}),(0,s.jsx)(n.td,{children:"Boolean"}),(0,s.jsx)(n.td,{children:"Is Open"})]})})]})]})}function h(e={}){const{wrapper:n}={...(0,o.R)(),...e.components};return n?(0,s.jsx)(n,{...e,children:(0,s.jsx)(a,{...e})}):a(e)}},2409:(e,n,r)=>{r.d(n,{A:()=>s});const s="data:image/png;base64,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"},8453:(e,n,r)=>{r.d(n,{R:()=>d,x:()=>c});var s=r(6540);const o={},t=s.createContext(o);function d(e){const n=s.useContext(t);return s.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function c(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(o):e.components||o:d(e.components),s.createElement(t.Provider,{value:n},e.children)}}}]);