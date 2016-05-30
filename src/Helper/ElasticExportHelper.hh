<?hh // strict
namespace ElasticExport\Helper;

use Plenty\Modules\Category\Contracts\CategoryBranchMarketplaceRepositoryContract;
use Plenty\Modules\Category\Contracts\CategoryBranchRepositoryContract;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Category\Models\CategoryBranch;
use Plenty\Modules\Item\Unit\Contracts\UnitLangRepositoryContract;
use Plenty\Modules\Item\Unit\Models\UnitLang;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueLangRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueLang;
use Plenty\Modules\Item\Character\Contracts\CharacterItemNameRepositoryContract;
use Plenty\Modules\Helper\Contracts\UrlBuilderRepositoryContract;
use Plenty\Modules\Category\Contracts\CategoryRepository;
use Plenty\Modules\Category\Models\Category;
use Plenty\Modules\Item\Character\Contracts\CharacterMarketComponentRepositoryContract;
use Plenty\Modules\Item\Character\Models\CharacterMarketComponent;
use Plenty\Modules\Item\DataLayer\Models\ItemCharacter;
use Plenty\Modules\Order\Shipping\DefaultShipping\Models\DefaultShipping;
use Plenty\Modules\Order\Payment\Method\Contracts\PaymentMethodRepositoryContract;
use Plenty\Modules\Order\Payment\Method\Models\PaymentMethod;
use Plenty\Modules\Item\DefaultShippingCost\Contracts\DefaultShippingCostRepositoryContract;
use Plenty\Modules\Item\Availability\Models\Availability;
use Plenty\Modules\Item\Availability\Models\AvailabilityLang;
use Plenty\Plugin\ConfigRepository;
use Plenty\Modules\Shipping\Countries\Contracts\CountryRepositoryContract;
use Plenty\Modules\Shipping\Countries\Models\Country;
/**
 * Class ElasticExportHelper
 * @package ElasticExportHelper\Helper
 */
class ElasticExportHelper
{
    const string SHIPPING_COST_TYPE_FLAT = 'flat';
    const string SHIPPING_COST_TYPE_CONFIGURATION = 'configuration';

    const string IMAGE_POSITION0 = 'position0';
    const string IMAGE_FIRST = 'firstImage';

    const int REMOVE_HTML_TAGS = 1;
    const int KEEP_HTML_TAGS = 0;

    const int ITEM_URL_NO = 0;
    const int ITEM_URL_YES = 1;

    const int TRANSFER_ITEM_AVAILABILITY_NO = 0;
    const int TRANSFER_ITEM_AVAILABILITY_YES = 1;

    const int TRANSFER_OFFER_PRICE_NO = 0;
    const int TRANSFER_OFFER_PRICE_YES = 1;

    const int TRANSFER_RRP_YES = 1;
    const int TRANSFER_RRP_NO = 0;

    const string BARCODE_EAN = 'EAN_13';
    const string BARCODE_ISBN = 'ISBN';

    /**
     * CategoryBranchRepositoryContract $categoryBranchRepository
     */
    private CategoryBranchRepositoryContract $categoryBranchRepository;

    /**
     * UnitLangRepositoryContract $unitLangRepository
     */
    private UnitLangRepositoryContract $unitLangRepository;

	/**
	 * AttributeValueLangRepositoryContract $attributeValueLangRepository
	 */
    private AttributeValueLangRepositoryContract $attributeValueLangRepository;

    /**
     * CharacterItemNameRepositoryContract $characterItemNameRepository
     */
    private CharacterItemNameRepositoryContract $characterItemNameRepository;

    /**
     * CategoryBranchMarketplaceRepositoryContract $categoryBranchMarketplaceRepository
     */
    private CategoryBranchMarketplaceRepositoryContract $categoryBranchMarketplaceRepository;

    /**
     * UrlBuilderRepositoryContract $urlBuilderRepository
     */
    private UrlBuilderRepositoryContract $urlBuilderRepository;

    /**
     * CategoryRepository $categoryRepository
     */
    private CategoryRepository $categoryRepository;

    /**
     * CharacterMarketComponentRepositoryContract $characterMarketComponentRepository;s
     */
    private CharacterMarketComponentRepositoryContract $characterMarketComponentRepository;

    /**
     * @var PaymentMethodRepositoryContract $paymentMethodRepository
     */
    private PaymentMethodRepositoryContract $paymentMethodRepository;

    /**
     * @var DefaultShippingCostRepositoryContract $defaultShippingCostRepository
     */
    private DefaultShippingCostRepositoryContract $defaultShippingCostRepository;    

    /**
     * @var ConfigRepository $configRepository
     */
    private ConfigRepository $configRepository;

    /**
     * CountryRepositoryContract $countryRepository
     */
    private CountryRepositoryContract $countryRepository;

    /**
     * ElasticExportHelper constructor.
     *
     * @param CategoryBranchRepositoryContract $categoryBranchRepository
     * @param UnitLangRepositoryContract $unitLangRepository
     * @param AttributeValueLangRepositoryContract $attributeValueLangRepository
     * @param CharacterItemNameRepositoryContract $characterItemNameRepository
     * @param CategoryBranchMarketplaceRepositoryContract $categoryBranchMarketplaceRepository
     * @param UrlBuilderRepositoryContract $urlBuilderRepository
     * @param CategoryRepository $categoryRepository
     * @param CharacterMarketComponentRepositoryContract $characterMarketComponentRepository     
     * @param PaymentMethodRepositoryContract $paymentMethodRepository     
     * @param ConfigRepository $configRepository
     * @param CountryRepositoryContract $countryRepository
     */
    public function __construct(CategoryBranchRepositoryContract $categoryBranchRepository,
                                UnitLangRepositoryContract $unitLangRepository,
                                AttributeValueLangRepositoryContract $attributeValueLangRepository,
                                CharacterItemNameRepositoryContract $characterItemNameRepository,
                                CategoryBranchMarketplaceRepositoryContract $categoryBranchMarketplaceRepository,
                                UrlBuilderRepositoryContract $urlBuilderRepository,
                                CategoryRepository $categoryRepository,
                                CharacterMarketComponentRepositoryContract $characterMarketComponentRepository,                                
                        		PaymentMethodRepositoryContract $paymentMethodRepository,
                                DefaultShippingCostRepositoryContract $defaultShippingCostRepository,                                
                                ConfigRepository $configRepository,
                                CountryRepositoryContract $countryRepository
    )
    {
        $this->categoryBranchRepository = $categoryBranchRepository;

        $this->unitLangRepository = $unitLangRepository;

        $this->attributeValueLangRepository = $attributeValueLangRepository;

        $this->characterItemNameRepository = $characterItemNameRepository;

        $this->categoryBranchMarketplaceRepository = $categoryBranchMarketplaceRepository;

        $this->urlBuilderRepository = $urlBuilderRepository;

        $this->categoryRepository = $categoryRepository;

        $this->characterMarketComponentRepository = $characterMarketComponentRepository;

		$this->paymentMethodRepository = $paymentMethodRepository;

        $this->defaultShippingCostRepository = $defaultShippingCostRepository;        

        $this->configRepository = $configRepository;

        $this->countryRepository = $countryRepository;
    }

    /**
     * Get name.
     *
     * @param  Record    $item
     * @param  KeyValue  $settings
     * @param  int $defaultNameLength
     * @return string
     */
    public function getName(Record $item, KeyValue $settings, int $defaultNameLength = 0):string
	{
		switch($settings->get('nameId'))
		{
			case 3:
				$name = $item->itemDescription->name3;
                break;

			case 2:
				$name = $item->itemDescription->name2;
                break;

			case 1:
			default:
				$name = $item->itemDescription->name1;
                break;
		}

        return $this->cleanName($name, $settings->get('nameMaxLength') ? $settings->get('nameMaxLength') : $defaultNameLength);
    }

    /**
     * Clean name to a defined length. If maxLength is 0 than named is returned intact.
     * @param  string name
     * @param  int maxLength
     * @return string
     */
    public function cleanName(string $name, int $maxLength = 0):string
    {
        $name = html_entity_decode($name);

        if($maxLength <= 0)
        {
            return $name;
        }

        return substr($name, 0, $maxLength);
    }

    /**
     * Get preview text.
     *
     * @param  Record        $item
     * @param  KeyValue      $settings
     * @param  int           $defaultPreviewTextLength
     * @return string
     */
    public function getPreviewText(Record $item, KeyValue $settings, int $defaultPreviewTextLength = 0):string
    {
        switch($settings->get('previewTextType'))
        {
            case 'itemShortDescription':
                $previewText = $item->itemDescription->shortDescription;
                break;

            case 'technicalData':
                $previewText = $item->itemDescription->technicalData;
                break;

            case 'itemDescriptionAndTechnicalData':
                $previewText = $item->itemDescription->description . ' ' . $item->itemDescription->technicalData;
                break;

            case 'itemDescription':
                $previewText = $item->itemDescription->description;
                break;

            case 'dontTransfer':
            default:
                $previewText = '';
                break;
        }

        $previewTextLength = $settings->get('previewTextMaxLength') ? $settings->get('previewTextMaxLength') : $defaultPreviewTextLength;

        if($settings->get('previewTextRemoveHtmlTags') == self::REMOVE_HTML_TAGS)
        {
            $previewText = strip_tags($previewText, str_replace([',', ' '], '', $settings->get('previewTextAllowHtmlTags')));
        }

        if($previewTextLength <= 0)
        {
            return $previewText;
        }

        return substr($previewText, 0, $previewTextLength);
    }

    /**
     * Get description.
     *
     * @param  Record        $item
     * @param  KeyValue      $settings
     * @param  int           $defaultDescriptionLength
     * @return string
     */
    public function getDescription(Record $item, KeyValue $settings, int $defaultDescriptionLength = 0):string
    {
        switch($settings->get('descriptionType'))
        {
            case 'itemShortDescription':
                $description = $item->itemDescription->shortDescription;
                break;

            case 'technicalData':
                $description = $item->itemDescription->technicalData;
                break;

            case 'itemDescriptionAndTechnicalData':
                $description = $item->itemDescription->description . ' ' . $item->itemDescription->technicalData;
                break;

            case 'itemDescription':
            default:
                $description = $item->itemDescription->description;
                break;
        }

        $descriptionLength = $settings->get('descriptionMaxLength') ? $settings->get('descriptionMaxLength') : $defaultDescriptionLength;

        if($settings->get('descriptionRemoveHtmlTags') == self::REMOVE_HTML_TAGS)
        {
            $description = strip_tags($description, str_replace([',', ' '], '', $settings->get('previewTextAllowHtmlTags')));
        }

        $description = html_entity_decode($description);

        if($descriptionLength <= 0)
        {
            return $description;
        }

        return substr($description, 0, $descriptionLength);
    }

    /**
	 * Get variation availability days.
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @return mixed
	 */
	public function getAvailability(Record $item, KeyValue $settings, bool $returnAvailabilityName = true):mixed
	{
        if($settings->get('transferItemAvailability') == self::TRANSFER_ITEM_AVAILABILITY_YES)
		{
            $availabilityIdString = 'itemAvailability' . $item->variationBase->availability;

		    return $settings->get($availabilityIdString);
		}

        $availability = $this->getAvailabilityData($item->variationBase->availability < 0 ? 10 : (int) $item->variationBase->availability);

        if($availability instanceof Availability)
        {
            $name = $this->getAvailabilityName($availability, $settings->get('lang'));

            if($returnAvailabilityName && strlen($name) > 0)
            {
                return $name;
            }
            elseif(!$returnAvailabilityName && $availability->averageDays > 0)
            {
                return (int) $availability->averageDays;
            }
        }

		return '';
	}

    /**
     * Get availability data.
     * @param int availabilityId
     * @return Availability|null
     */
    public function getAvailabilityData(int $availabilityId):?Availability
    {        
        $availabilities = $this->getConfig('plenty.item.availability');

        foreach($availabilities as $availability)
        {
            if($availability instanceof Availability && $availability->id == $availabilityId)
            {
                return $availability;
            }
        }        

        return null;
    }

    /**
     * Get availability name for a vigen availability and lang.
     * @param Availability $availability
     * @param string $lang
     * @return string
     */
    private function getAvailabilityName(Availability $availability, string $lang):string
    {
        foreach($availability->langs as $availabilityLang)
        {
            if($availabilityLang->lang == $lang)
            {
                return $availabilityLang->name;
            }
        }

        return '';
    }

    /**
     * Get the item URL.
     * @param  Record $item
     * @param  KeyValue $settings
     * @param  ?bool $addReferrer = true  Choose if referrer id should be added as parameter.
     * @param  ?bool $useIntReferrer = false Choose if referrer id should be used as integer.
     * @param  ?bool $useHttps = true Choose if https protocol should be used.
     * @return string Item url.
     */
    public function getUrl(Record $item, KeyValue $settings, bool $addReferrer = true, bool $useIntReferrer = false, bool $useHttps = true):string
	{
        if($settings->get('itemUrl') == self::ITEM_URL_NO)
        {
            return '';
        }

		$urlParams = [];

        $link = $this->urlBuilderRepository->getItemUrl($item->itemBase->id, $settings->get('plentyId'), $item->itemDescription->urlContent, $settings->get('lang') ? $settings->get('lang') : 'de');

		if($addReferrer && $settings->get('referrerId'))
		{
            $urlParams[] = 'ReferrerID=' . ($useIntReferrer ? (int) $settings->get('referrerId') : $settings->get('referrerId'));
		}

		if(strlen($settings->get('urlParam')))
		{
			$urlParams[] = $settings->get('urlParam');
		}

		if (is_array($urlParams) && count($urlParams) > 0)
		{
			$link .= '?' . implode('&', $urlParams);
		}

		return $link;
	}

    /**
     * Get category branch for a custom category id.
     * @param  int $categoryId
     * @param  string $lang
     * @param  int $plentyId
     * @param  string $separator = ' > '
     * @return string
     */
    public function getCategory(int $categoryId, string $lang, int $plentyId, string $separator = ' > '):string
	{
        $categoryBranch = $this->categoryBranchRepository->findCategoryBranch($categoryId, $lang, $plentyId);

        if(!is_null($categoryBranch) && is_array($categoryBranch->branchName) && count($categoryBranch->branchName))
		{
			return implode($separator, $categoryBranch->branchName);
		}

		return '';
	}

    public function getCategoryBranch(Record $item, KeyValue $settings, int $categoryLevel):string
    {
        $categoryBranch = $this->categoryBranchRepository->findCategoryBranch($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId'));

        $category = null;

        $lang = $settings->get('lang') ? $settings->get('lang') : 'de';

        if(!is_null($categoryBranch) && is_array($categoryBranch->branch) && count($categoryBranch->branch))
        {
            switch($categoryLevel)
            {
                case 1:
                    $category = $this->categoryRepository->get($categoryBranch->plenty_category_branch_category1_id, $lang);
                    break;

                case 2:
                    $category = $this->categoryRepository->get($categoryBranch->plenty_category_branch_category2_id, $lang);
                    break;

                case 3:
                    $category = $this->categoryRepository->get($categoryBranch->plenty_category_branch_category3_id, $lang);
                    break;

                case 4:
                    $category = $this->categoryRepository->get($categoryBranch->plenty_category_branch_category4_id, $lang);
                    break;

                case 5:
                    $category = $this->categoryRepository->get($categoryBranch->plenty_category_branch_category5_id, $lang);
                    break;

                case 6:
                    $category = $this->categoryRepository->get($categoryBranch->plenty_category_branch_category6_id, $lang);
                    break;

            }
        }

        if($category instanceof Category)
        {
            foreach($category->details as $categoryDetails)
            {
                if($categoryDetails->lang == $lang)
                {
                    return $categoryDetails->name;
                }
            }
        }

        return '';
    }

    /**
     * Get shipping cost.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @return float
     */
    public function getShippingCost(Record $item, KeyValue $settings):float
    {
        if($settings->get('shippingCostType') == self::SHIPPING_COST_TYPE_FLAT)
        {
            return (float) $settings->get('shippingCostFlat');
        }

        $defaultShipping = $this->getDefaultShipping($settings);

        if( $defaultShipping instanceof DefaultShipping &&
            $defaultShipping->shippingDestinationId)
        {
            $paymentMethodId = $defaultShipping->paymentMethod2;

            // 0 - is always "payment in advance" so we use always the second and third payment methods from the default shipping
            if($settings->get('shippingCostMethodOfPayment') == 2)
            {
                $paymentMethodId = $defaultShipping->paymentMethod3;
            }

            return $this->calculateShippingCost($item->itemBase->id, $defaultShipping->shippingDestinationId, $settings->get('referrerId'), $paymentMethodId);
        }

        return (float) $item->itemBase->defaultShippingCost;
    }

    /**
     * Calculate default shipping cost.
     * @param int $itemId
     * @param int $shippingDestinationId
     * @param float $referrerId
     * @param int $paymentMethodId
     * @return float
     */
    public function calculateShippingCost(int $itemId, int $shippingDestinationId, float $referrerId, int $paymentMethodId):float
    {
        return $this->defaultShippingCostRepository->findShippingCost($itemId, $referrerId, $shippingDestinationId, $paymentMethodId);
    }

    /**
     * returns the price of the given variation
     * @param  Record   $item
     * @return float
     */
    public function getPrice(Record $item):float
    {
            return $item->variationRetailPrice->price;
    }

    /**
     * returns the recommendedRetailPrice of the given variation if transferRrp is set
     * @param Record $item
     * @param KeyValue $settings
     * @return float
     */
    public function getRecommendedRetailPrice(Record $item, KeyValue $settings):float
    {
        if($settings->get('transferRrp') == self::TRANSFER_RRP_YES)
        {
            return $item->variationRecommendedRetailPrice->price;
        }

        return 0.0;
    }

    /**
     * returns the specialOfferPrice of the given variation if transferOfferPrice is set
     * @param Record $item
     * @param KeyValue $settings
     * @return float
     */
    public function getSpecialPrice(Record $item, KeyValue $settings):float
    {
        if($settings->get('transferOfferPrice') == self::TRANSFER_OFFER_PRICE_YES)
        {
            return $item->variationSpecialOfferRetailPrice->retailPrice;
        }

        return 0.0;
    }

    /**
     * Get the attribute value set short frontend name. Ex. blue, XL
     * @param  Record   $item
     * @param  KeyValue $settings
     * @return string
     */
    public function getAttributeValueSetShortFrontendName(Record $item, KeyValue $settings):string
    {
        $values = [];

        if($item->variationBase->attributeValueSetId)
        {
            foreach($item->variationAttributeValueList as $attribute)
            {
                $attributeValueLang = $this->attributeValueLangRepository->findAttributeValue($attribute->attributeValueId, $settings->get('lang') ? $settings->get('lang') : 'de');

                if($attributeValueLang instanceof AttributeValueLang)
                {
                    $values[] = $attributeValueLang->name;
                }

            }
        }

        return implode(', ', $values);
    }

    /**
     * Get base price.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $separator         =             '/'
     * @param  bool     $compact           =             false
     * @param  bool     $dotPrice          =             false
     * @param  string   $currency          =             ''
     * @param  float    $price             =             0.0
     * @return string
     */
    public function getBasePrice(
        Record $item,
        KeyValue $settings,
        string $separator = '/',
        bool $compact = false,
        bool $dotPrice = false,
        string $currency = '',
        float $price = 0.0
    ):string
	{
        $currency = strlen($currency) ? $currency : $this->getDefaultCurrency();
		$price = $price > 0 ? $price : (float) $item->variationRetailPrice->price;
        $lot = (int) $item->variationBase->content;
        $unitLang = $this->unitLangRepository->findUnit((int) $item->variationBase->unitId, $settings->get('lang') ? $settings->get('lang') : 'de');

        if($unitLang instanceof UnitLang)
        {
            $unitShortcut = $unitLang->unit->plenty_unit_unit_of_measurement;
            $unitName = $unitLang->plenty_unit_lang_name;
        }
        else
        {
            $unitShortcut = '';
            $unitName = '';
        }

		$basePriceDetails = $this->getBasePriceDetails($lot, $price, $unitShortcut);

		if((float) $basePriceDetails['price'] <= 0 || ((int) $basePriceDetails['lot'] <= 1 && $basePriceDetails['unit'] == 'C62'))
		{
			return '';
		}

		if ($dotPrice == true)
		{
			$basePriceDetails['price'] = number_format($basePriceDetails['price'], 2, '.', '');
		}
		else
		{
			$basePriceDetails['price'] = number_format($basePriceDetails['price'], 2, ',', '');
		}

		if ($compact == true)
		{
			return	'(' . (string) $basePriceDetails['price'] . $currency . $separator . (string) $basePriceDetails['lot'] . $unitShortcut . ')';
		}
		else
		{
			return	(string) $basePriceDetails['price'] . ' ' . $currency . $separator . (string) $basePriceDetails['lot'] . ' ' . $unitName;
		}
	}

	/**
	 * Get base price.
	 *
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @return Map
	 */
	public function getBasePriceList(Record $item, KeyValue $settings):Map<string,mixed>
	{
		$price = (float)$item->variationRetailPrice->price;
		$lot = (int)$item->variationBase->content;
		$unitLang = $this->unitLangRepository->findUnit((int)$item->variationBase->unitId, $settings->get('lang') ? $settings->get('lang') : 'de');

		if($unitLang instanceof UnitLang)
		{
            $unitShortcut = $unitLang->unit->plenty_unit_unit_of_measurement;
			$unitName = $unitLang->plenty_unit_lang_name;
		}
		else
		{
            $unitShortcut = '';
			$unitName = '';
		}

		$basePriceDetails = $this->getBasePriceDetails($lot, $price, $unitShortcut);

		$basePriceDetails['price'] = number_format($basePriceDetails['price'], 2, '.', '');

        return Map{'lot' => (int)$basePriceDetails['lot'], 'price' => (float)$basePriceDetails['price'], 'unit' => (string)$unitName};
	}

    /**
     * Get main image.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @return string
     */
    public function getMainImage(Record $item, KeyValue $settings):string
    {
        foreach($item->variationImageList as $image)
        {
            if($settings->get('imagePosition') == self::IMAGE_FIRST)
            {
                return $this->urlBuilderRepository->getImageUrl($image->path, $settings->get('plentyId'), 'normal', $image->fileType, $image->type == 'external');
            }
            elseif($settings->get('imagePosition')== self::IMAGE_POSITION0 && $image->position == 0)
            {
                return $this->urlBuilderRepository->getImageUrl($image->path, $settings->get('plentyId'), 'normal', $image->fileType, $image->type == 'external');
            }
        }

        return '';
    }

    /**
     * Get list of images for current item.
     * @param Record $item
     * @param KeyValue $settings
     * @param string $imageType = 'normal'
     * @return array<mixed>
     */
    public function getImageList(Record $item, KeyValue $settings, string $imageType = 'normal'):array<string>
    {
        $list = [];

        foreach($item->variationImageList as $image)
        {
            $list[] = $this->urlBuilderRepository->getImageUrl($image->path, $settings->get('plentyId'), $imageType, $image->fileType, $image->type == 'external');
        }

        return $list;
    }

    /**
     * Get item character value by backend name.
     * @param  Record $item
     * @param KeyValue $settings
     * @param  string $backendName
     * @return string
     */
    public function getItemCharacterByBackendName(Record $item, KeyValue $settings, string $backendName):string
    {
        foreach($item->itemCharacterList as $itemCharacter)
        {
            $itemCharacterBackendName = $this->characterItemNameRepository->findCharacterItem($itemCharacter->itemCharacterId, $settings->get('lang')? $settings->get('lang') : 'de');

            if($itemCharacterBackendName == $backendName)
            {
                return (string) $itemCharacter->characterValue;
            }
        }

        return '';
    }

    /**
     * Get item characters that match referrer from settings and a given component id.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  ?int     $componentId  = null
     * @return array<int, mixed>
     */
    public function getItemCharactersByComponent(Record $item, KeyValue $settings, ?int $componentId = null):Vector<array<string,mixed>>
    {
        $characterList = $item->itemCharacterList;

        $characterMarketComponents = $this->characterMarketComponentRepository->getCharacterMarketComponents($settings->get('referrerId'), !is_null($componentId) ? $componentId : null);

        $list = Vector{};

        foreach($characterList as $character)
		{
            foreach($characterMarketComponents as $characterMarketComponent)
            {
                if($characterMarketComponent instanceof CharacterMarketComponent && $characterMarketComponent->character_item_id == $character->characterId)
                {
                    $list[] = [
                        'itemCharacterId' => $character->itemCharacterId,
                        'characterId' => $character->characterId,
                        'characterValue' => $character->characterValue,
                        'characterValueType' => $character->characterValueType,
                        'characterItemId' => $characterMarketComponent->character_item_id,
                        'componentId' => $characterMarketComponent->component_id,
                        'referrerId' => $characterMarketComponent->market_reference,
                        'externalComponent' => $characterMarketComponent->external_component,
					];
                }
            }
		}

		return $list;
    }

    /**
     * Get barcode by a given type.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $barcodeType
     * @return string
     */
    public function getBarcodeByType(Record $item, KeyValue $settings, string $barcodeType):string
    {
        foreach($item->variationBarcodeList as $variationBarcode)
        {
            if($variationBarcode->barcodeType == $barcodeType)
            {
                return (string) $variationBarcode->code;
            }
        }

        return '';
    }

    /**
     * Get base price details.
     * @param  int    $lot
     * @param  float  $price
     * @param  string $unit
     * @return Map
     */
    private function getBasePriceDetails(int $lot, float $price, string $unit):Map<string,mixed>
    {
        $lot = $lot == 0 ? 1 : $lot; // TODO  PlentyStringUtils::numberFormatLot($lot, true);
		$basePrice = 0;
		$basePriceLot = 1;
        $unit = strlen($unit) ? $unit : 'C62';
        $basePriceUnit = $unit;

		$factor = 1.0;

		if($unit == 'LTR' || $unit == 'KGM')
		{
			$basePriceLot = 1;
		}
		elseif($unit == 'GRM' || $unit == 'MLT')
		{
			if($lot <= 250)
			{
				$basePriceLot = 100;
			}
			else
			{
				$factor = 1000.0;
				$basePriceLot = 1;
				$basePriceUnit = $unit =='GRM' ? 'KGM' : 'LTR';
			}
		}
		else
		{
			$basePriceLot = 1;
		}

		$endLot = ($basePriceLot/$lot);

		return Map{'lot' => (int) $basePriceLot, 'price' => (float) $price * $factor * $endLot, 'unit' => (string) $basePriceUnit};
    }

    /**
     * Get default currency from configuration.
     * @return string
     */
    public function getDefaultCurrency():string
    {
        $config = []; // TODO load config

        if(is_array($config) && is_string($config['cfgCurrency']))
        {
            return $config['cfgCurrency'];
        }

        return 'EUR';
    }

    /**
     * Get list of payment methods.
     * @param KeyValue $settings
     * @return Map<int,PaymentMethod>
     */
    public function getPaymentMethods(KeyValue $settings):Map<int,PaymentMethod>
    {
        $paymentMethods = $this->paymentMethodRepository->getPaymentMethods($settings->get('destination'), $settings->get('plentyId'), $settings->get('lang'));

        $list = Map{};

        foreach($paymentMethods as $paymentMethod)
        {
            $list[$paymentMethod->id] = $paymentMethod;
        }

        return $list;
    }

    /**
	 * Get the default shipping.
	 * @param  KeyValue $settings
	 * @return DefaultShipping|null
	 */
	public function getDefaultShipping(KeyValue $settings):?DefaultShipping
	{
        $defaultShippingProfiles = $this->getConfig('plenty.order.shipping.default_shipping');

        foreach($defaultShippingProfiles as $defaultShippingProfile)
        {
            if($defaultShippingProfile instanceof DefaultShipping && $defaultShippingProfile->id == $settings->get('shippingCostConfiguration'))
            {
                return $defaultShippingProfile;
            }
        }        
        
        return null;
	}

    /**
     * Get custom configuration.
     * @param  string $key
     * @param  mixed $default = null
     * @return T
     */
    public function getConfig<T>(string $key, mixed $default = null):T
    {
        return $this->configRepository->get($key, $default);
    }

    /**
     * @param KeyValue $settings
     * @param string $isoCodeType
     * @return string
     */
    public function getCountry(KeyValue $settings, string $isoCodeType):string
    {
        $country = $this->countryRepository->findIsoCode($settings->get('destination'), $isoCodeType);

        return $country;
    }
}
