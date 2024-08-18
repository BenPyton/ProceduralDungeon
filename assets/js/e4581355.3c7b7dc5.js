"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[6769],{6749:(e,n,s)=>{s.r(n),s.d(n,{assets:()=>t,contentTitle:()=>l,default:()=>h,frontMatter:()=>r,metadata:()=>i,toc:()=>c});var o=s(4848),d=s(8453);const r={pagination_prev:null,pagination_next:null},l="Console Commands",i={id:"guides/Console-Commands",title:"Console Commands",description:"The plugin adds several console commands to control some plugin's settings and add some debug functions.",source:"@site/docs/guides/Console-Commands.md",sourceDirName:"guides",slug:"/guides/Console-Commands",permalink:"/ProceduralDungeon/guides/Console-Commands",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},t={},c=[{value:"Console Variables",id:"console-variables",level:2},{value:"Console Commands",id:"console-commands-1",level:2}];function a(e){const n={a:"a",code:"code",em:"em",h1:"h1",h2:"h2",p:"p",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,d.R)(),...e.components};return(0,o.jsxs)(o.Fragment,{children:[(0,o.jsx)(n.h1,{id:"console-commands",children:"Console Commands"}),"\n",(0,o.jsx)(n.p,{children:"The plugin adds several console commands to control some plugin's settings and add some debug functions."}),"\n",(0,o.jsx)(n.h2,{id:"console-variables",children:"Console Variables"}),"\n",(0,o.jsxs)(n.p,{children:["All console variables begins with ",(0,o.jsx)(n.code,{children:"pd"})," (for ",(0,o.jsx)(n.em,{children:(0,o.jsx)(n.strong,{children:"p"})}),"rocedural ",(0,o.jsx)(n.em,{children:(0,o.jsx)(n.strong,{children:"d"})}),"ungeon), then continue with categories (e.g. ",(0,o.jsx)(n.code,{children:"Occlusion"})," or ",(0,o.jsx)(n.code,{children:"Debug"}),"), then sub-categories, etc. and finally the variable."]}),"\n",(0,o.jsxs)(n.p,{children:["Here a list of ",(0,o.jsx)(n.em,{children:"all"})," console variables added by the plugin:"]}),"\n",(0,o.jsxs)(n.table,{children:[(0,o.jsx)(n.thead,{children:(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.th,{children:"Console Variable"}),(0,o.jsx)(n.th,{children:"Arguments"}),(0,o.jsx)(n.th,{children:"Description"})]})}),(0,o.jsxs)(n.tbody,{children:[(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.code,{children:"pd.Occlusion"})}),(0,o.jsx)(n.td,{children:"boolean"}),(0,o.jsxs)(n.td,{children:["Enable/disable the plugin's ",(0,o.jsx)(n.a,{href:"/ProceduralDungeon/guides/Occlusion-Culling",children:"room culling"})," system."]})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.code,{children:"pd.Occlusion.Distance"})}),(0,o.jsx)(n.td,{children:"integer"}),(0,o.jsxs)(n.td,{children:["Change the number of visible rooms from the player's room (",(0,o.jsx)(n.code,{children:"1"})," means only the player's room)."]})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.code,{children:"pd.Occlusion.DynamicActors"})}),(0,o.jsx)(n.td,{children:"boolean"}),(0,o.jsxs)(n.td,{children:["Enable/disable the occlusion of actors with a ",(0,o.jsx)(n.code,{children:"Room Visibility"})," component attached on them."]})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.code,{children:"pd.Debug.Draw"})}),(0,o.jsx)(n.td,{children:"boolean"}),(0,o.jsx)(n.td,{children:"Enable/disable the debug drawings of the rooms and doors."})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.code,{children:"pd.Debug.Log.OnScreen"})}),(0,o.jsx)(n.td,{children:"boolean"}),(0,o.jsx)(n.td,{children:"Enable/disable the on-screen logging of the plugin."})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.code,{children:"pd.Debug.Log.Duration"})}),(0,o.jsx)(n.td,{children:"float"}),(0,o.jsx)(n.td,{children:"Change the on-screen logging duration (in seconds) of the plugin."})]})]})]}),"\n",(0,o.jsx)(n.h2,{id:"console-commands-1",children:"Console Commands"}),"\n",(0,o.jsxs)(n.p,{children:["Here a list of ",(0,o.jsx)(n.em,{children:"all"})," console commands added by the plugin:"]}),"\n",(0,o.jsxs)(n.table,{children:[(0,o.jsx)(n.thead,{children:(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.th,{children:"Console Command"}),(0,o.jsx)(n.th,{children:"Arguments"}),(0,o.jsx)(n.th,{children:"Description"})]})}),(0,o.jsxs)(n.tbody,{children:[(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.code,{children:"pd.Generate"})}),(0,o.jsx)(n.td,{children:"string\xa0(optional)"}),(0,o.jsxs)(n.td,{children:["Call ",(0,o.jsx)(n.code,{children:"Generate"})," on ",(0,o.jsx)(n.a,{href:"/ProceduralDungeon/guides/Dungeon-Generator",children:"Dungeon Generator"})," actors with the name or tag provided, or all if nothing provided."]})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.code,{children:"pd.Unload"})}),(0,o.jsx)(n.td,{children:"string\xa0(optional)"}),(0,o.jsxs)(n.td,{children:["Call ",(0,o.jsx)(n.code,{children:"Unload"})," on ",(0,o.jsx)(n.a,{href:"/ProceduralDungeon/guides/Dungeon-Generator",children:"Dungeon Generator"})," actors with the name or tag provided, or all if nothing provided."]})]})]})]})]})}function h(e={}){const{wrapper:n}={...(0,d.R)(),...e.components};return n?(0,o.jsx)(n,{...e,children:(0,o.jsx)(a,{...e})}):a(e)}},8453:(e,n,s)=>{s.d(n,{R:()=>l,x:()=>i});var o=s(6540);const d={},r=o.createContext(d);function l(e){const n=o.useContext(r);return o.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function i(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(d):e.components||d:l(e.components),o.createElement(r.Provider,{value:n},e.children)}}}]);