"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[127],{4662:(e,n,o)=>{o.r(n),o.d(n,{assets:()=>a,contentTitle:()=>s,default:()=>l,frontMatter:()=>r,metadata:()=>u,toc:()=>d});var t=o(4848),i=o(8453);const r={pagination_prev:"guides/Choose-Next-Room-Data",pagination_next:"guides/Is-Valid-Dungeon"},s="Continue To Add Room",u={id:"guides/Continue-To-Add-Room",title:"Continue To Add Room",description:"This function determine if the generation should continue or not.",source:"@site/docs/guides/Continue-To-Add-Room.md",sourceDirName:"guides",slug:"/guides/Continue-To-Add-Room",permalink:"/ProceduralDungeon/guides/Continue-To-Add-Room",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{pagination_prev:"guides/Choose-Next-Room-Data",pagination_next:"guides/Is-Valid-Dungeon"},sidebar:"homeSidebar",previous:{title:"Choose Next Room",permalink:"/ProceduralDungeon/guides/Choose-Next-Room-Data"},next:{title:"Is Valid Dungeon",permalink:"/ProceduralDungeon/guides/Is-Valid-Dungeon"}},a={},d=[];function c(e){const n={br:"br",h1:"h1",img:"img",p:"p",strong:"strong",...(0,i.R)(),...e.components};return(0,t.jsxs)(t.Fragment,{children:[(0,t.jsx)(n.h1,{id:"continue-to-add-room",children:"Continue To Add Room"}),"\n",(0,t.jsx)(n.p,{children:"This function determine if the generation should continue or not."}),"\n",(0,t.jsxs)(n.p,{children:["A generation will naturally stops if there are no unconnected door left.",(0,t.jsx)(n.br,{}),"\n","But in most cases this is not happening and you have to stop the generation before it spawns too many rooms.",(0,t.jsx)(n.br,{}),"\n","So you have to return false in this function to stop the generation."]}),"\n",(0,t.jsx)(n.p,{children:"In output, you have to return true to continue the generation, or false to stop it."}),"\n",(0,t.jsxs)(n.p,{children:["Here an example where we let the generation ends naturally (",(0,t.jsx)(n.strong,{children:"be careful of infinite loop"}),"):"]}),"\n",(0,t.jsx)(n.p,{children:(0,t.jsx)(n.img,{src:o(2884).A+"",width:"426",height:"140"})})]})}function l(e={}){const{wrapper:n}={...(0,i.R)(),...e.components};return n?(0,t.jsx)(n,{...e,children:(0,t.jsx)(c,{...e})}):c(e)}},2884:(e,n,o)=>{o.d(n,{A:()=>t});const t=o.p+"assets/images/ContinueAddingRoom-fa9c72c39ec382710919c982e8a4fb94.jpg"},8453:(e,n,o)=>{o.d(n,{R:()=>s,x:()=>u});var t=o(6540);const i={},r=t.createContext(i);function s(e){const n=t.useContext(r);return t.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function u(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(i):e.components||i:s(e.components),t.createElement(r.Provider,{value:n},e.children)}}}]);