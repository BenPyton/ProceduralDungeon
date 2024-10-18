"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[6537],{86641:(e,n,r)=>{r.r(n),r.d(n,{assets:()=>l,contentTitle:()=>o,default:()=>h,frontMatter:()=>c,metadata:()=>s,toc:()=>d});var i=r(74848),t=r(28453);const c={title:"Api References",pagination_prev:null,pagination_next:null},o="Contribute to the API References",s={id:"contributing/api-references",title:"Api References",description:"You can contribute to the API documentation by adding or improving comments directly in the plugin's source code.",source:"@site/docs/contributing/api-references.md",sourceDirName:"contributing",slug:"/contributing/api-references",permalink:"/ProceduralDungeon/contributing/api-references",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{title:"Api References",pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},l={},d=[{value:"Display Name",id:"display-name",level:3},{value:"Full Description",id:"full-description",level:3},{value:"Short Description",id:"short-description",level:3},{value:"Function Parameters",id:"function-parameters",level:3}];function a(e){const n={br:"br",code:"code",h1:"h1",h3:"h3",header:"header",hr:"hr",p:"p",pre:"pre",strong:"strong",...(0,t.R)(),...e.components};return(0,i.jsxs)(i.Fragment,{children:[(0,i.jsx)(n.header,{children:(0,i.jsx)(n.h1,{id:"contribute-to-the-api-references",children:"Contribute to the API References"})}),"\n",(0,i.jsx)(n.p,{children:"You can contribute to the API documentation by adding or improving comments directly in the plugin's source code."}),"\n",(0,i.jsx)(n.hr,{}),"\n",(0,i.jsx)(n.h3,{id:"display-name",children:"Display Name"}),"\n",(0,i.jsxs)(n.p,{children:["Sometimes, the default display name will not be correct.\r\nFor example, an enum named ",(0,i.jsx)(n.code,{children:"EMyEnum"})," will have a display name ",(0,i.jsx)(n.code,{children:"EMy Enum"}),".",(0,i.jsx)(n.br,{}),"\n","In those cases, we can define a custom display name by adding a ",(0,i.jsx)(n.code,{children:"DisplayName"})," meta specifier to the UE macro:"]}),"\n",(0,i.jsx)(n.pre,{children:(0,i.jsx)(n.code,{className:"language-cpp",children:'UCLASS(Blueprintable, meta = (DisplayName = "My Great Actor!"))\r\nclass AMyActor : public AActor\r\n{\r\n\t...\r\n}\n'})}),"\n",(0,i.jsxs)(n.p,{children:["Available for:\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UCLASS"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f USTRUCT"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UINTERFACE"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UENUM"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UMETA"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UFUNCTION"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UPROPERTY"})]}),"\n",(0,i.jsx)(n.hr,{}),"\n",(0,i.jsx)(n.h3,{id:"full-description",children:"Full Description"}),"\n",(0,i.jsxs)(n.p,{children:["The full description is used to describe in details what the class/function/variable is used for.",(0,i.jsx)(n.br,{}),"\n","Markdown syntax can and ",(0,i.jsx)(n.strong,{children:"should"})," be used when necessary."]}),"\n",(0,i.jsx)(n.p,{children:"Add a standard comment above the UE macro:"}),"\n",(0,i.jsx)(n.pre,{children:(0,i.jsx)(n.code,{className:"language-cpp",children:"// Here is the description of the variable.\r\n// *It can be on multiple lines.*\r\nUPROPERTY(BlueprintReadWrite)\r\nint MyVariable;\n"})}),"\n",(0,i.jsxs)(n.p,{children:["Available for:\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UCLASS"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f USTRUCT"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UINTERFACE"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UENUM"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UMETA"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UFUNCTION"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UPROPERTY"})]}),"\n",(0,i.jsx)(n.hr,{}),"\n",(0,i.jsx)(n.h3,{id:"short-description",children:"Short Description"}),"\n",(0,i.jsx)(n.p,{children:"The short description is a one-liner to tell roughly what the class/function/variable is used for.\\"}),"\n",(0,i.jsxs)(n.p,{children:["Add the ",(0,i.jsx)(n.code,{children:"ShortTooltip"})," meta specifier to the UE macro."]}),"\n",(0,i.jsx)(n.pre,{children:(0,i.jsx)(n.code,{className:"language-cpp",children:'USTRUCT(BlueprintType, meta = (ShortTooltip = "Just a short one-line description here."))\r\nstruct FMyStruct\r\n{\r\n\t...\r\n}\n'})}),"\n",(0,i.jsxs)(n.p,{children:["Available for:\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UCLASS"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f USTRUCT"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UINTERFACE"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UENUM"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UMETA"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UFUNCTION"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UPROPERTY"})]}),"\n",(0,i.jsx)(n.hr,{}),"\n",(0,i.jsx)(n.h3,{id:"function-parameters",children:"Function Parameters"}),"\n",(0,i.jsx)(n.p,{children:"The function parameters,as well as the return value, can get a description too."}),"\n",(0,i.jsx)(n.p,{children:"Use the Doxygen syntax in the comment:"}),"\n",(0,i.jsx)(n.pre,{children:(0,i.jsx)(n.code,{className:"language-cpp",children:"// This is the full description of the function\r\n// @param SomeActor This is the description for the SomeActor parameter.\r\n// @param SomeValue Description for the SomeValue input.\r\n// @return The description for the return value.\r\nUFUNCTION(BlueprintCallable)\r\nint MyFunction(AActor* SomeActor, int SomeValue)\r\n{\r\n\t...\r\n}\n"})}),"\n",(0,i.jsxs)(n.p,{children:["Available for:\r\n",(0,i.jsx)(n.code,{children:"\u274c UCLASS"}),"\r\n",(0,i.jsx)(n.code,{children:"\u274c USTRUCT"}),"\r\n",(0,i.jsx)(n.code,{children:"\u274c UINTERFACE"}),"\r\n",(0,i.jsx)(n.code,{children:"\u274c UENUM"}),"\r\n",(0,i.jsx)(n.code,{children:"\u274c UMETA"}),"\r\n",(0,i.jsx)(n.code,{children:"\u2714\ufe0f UFUNCTION"}),"\r\n",(0,i.jsx)(n.code,{children:"\u274c UPROPERTY"})]})]})}function h(e={}){const{wrapper:n}={...(0,t.R)(),...e.components};return n?(0,i.jsx)(n,{...e,children:(0,i.jsx)(a,{...e})}):a(e)}},28453:(e,n,r)=>{r.d(n,{R:()=>o,x:()=>s});var i=r(96540);const t={},c=i.createContext(t);function o(e){const n=i.useContext(c);return i.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function s(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(t):e.components||t:o(e.components),i.createElement(c.Provider,{value:n},e.children)}}}]);